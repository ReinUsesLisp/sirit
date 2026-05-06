"""Generate src/instructions/_generated.cpp from sirit.h + spirv.core.grammar.json.

Cross-references each public `Module::*` method declaration in
include/sirit/sirit.h against the SPIR-V core grammar. For every method that
maps to a single direct opcode emit, this script emits the corresponding .cpp
body using the operand list from the grammar.

The hand-written header remains the source of truth for the API surface
(method names, parameter names, default values, ergonomic template overloads).
The generator only fills in the mechanical bodies, where most past bugs lived
(wrong opcode constants, wrong OpId form, wrong operand order).

What is skipped (intentionally):
  - Template variadic overloads (defined inline in the header).
  - Inline definitions in the header (have no .cpp counterpart).
  - Multi-statement emits: DeferredOpPhi, OpBranchConditional, OpSwitch.
  - Module-level state plumbing: AddCapability, AddExtension, SetMemoryModel,
    AddEntryPoint, AddExecutionMode, AddLabel, AddLocalVariable,
    AddGlobalVariable, Assemble, PatchDeferredPhi, GetGLSLstd450.
  - Name aliases / wrappers: TypeSInt, TypeUInt, OpDemoteToHelperInvocationEXT.
  - GLSL.std.450 extended-instruction wrappers (OpFAbs, OpSin, ...): they go
    through OpExtInst and are produced by a different macro in extension.cpp.
  - OpLabel() (returns an id without emitting).

Output: src/instructions/_generated.cpp. Inspect, then decide whether to wire
in or replace existing files.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HEADER_PATH = ROOT / "include" / "sirit" / "sirit.h"
GRAMMAR_PATH = (
    ROOT / "externals" / "SPIRV-Headers" / "include" / "spirv" / "unified1"
    / "spirv.core.grammar.json"
)
OUTPUT_PATH = ROOT / "src" / "instructions" / "_generated.cpp"


# ---------------------------------------------------------------------------
# Skip list and method-name -> grammar-opname mapping
# ---------------------------------------------------------------------------

SKIP = {
    # Module-level setup, multi-statement, or non-emitting helpers.
    "AddCapability", "AddExtension", "SetMemoryModel", "AddEntryPoint",
    "AddExecutionMode", "AddLabel", "AddLocalVariable", "AddGlobalVariable",
    "Assemble", "PatchDeferredPhi",
    "GetGLSLstd450", "GetNonSemanticDebugPrintf",
    "GetAmdGcnShader", "GetAmdShaderTrinaryMinMax", "GetAmdExplicitVertexParameter",
    # Multi-statement emits.
    "DeferredOpPhi", "OpBranchConditional", "OpSwitch",
    # Wrappers / aliases (no direct opcode mapping).
    "TypeSInt", "TypeUInt", "OpDemoteToHelperInvocationEXT",
    # Convention: these public methods return their input id (target / type)
    # rather than the `<< EndOp{}` result, which the generic emit pattern
    # cannot express. Keep hand-written.
    "Name", "MemberName",
    # OpLabel returns an id without emitting.
    "OpLabel",
    # OpDebugPrintf prepends `format` onto a hand-built operands vector and
    # routes through TypeVoid(); the simple wrapper template can't express it.
    "OpDebugPrintf",
}


# Method-name -> grammar-opname overrides for cases where the public API
# renames an opcode (e.g. drops the EXT suffix for ergonomics).
OPNAME_OVERRIDES = {
    "OpAtomicFMax": "OpAtomicFMaxEXT",
    "OpAtomicFMin": "OpAtomicFMinEXT",
}


# Extended-instruction sets known to the generator. Each entry maps
#   grammar file (in the SPIR-V-Headers submodule)
#   -> the C++ constant prefix used by the matching header
#   -> the Module::Get<X>() helper that returns the imported set Id.
EXTINST_SETS = [
    {
        "file": "extinst.glsl.std.450.grammar.json",
        "prefix": "GLSLstd450",
        "getter": "GetGLSLstd450",
    },
    {
        "file": "extinst.spv-amd-gcn-shader.grammar.json",
        "prefix": "AMD_gcn_shader",
        "getter": "GetAmdGcnShader",
    },
    {
        "file": "extinst.spv-amd-shader-trinary-minmax.grammar.json",
        "prefix": "AMD_shader_trinary_minmax",
        "getter": "GetAmdShaderTrinaryMinMax",
    },
    {
        "file": "extinst.spv-amd-shader-explicit-vertex-parameter.grammar.json",
        "prefix": "AMD_shader_explicit_vertex_parameter",
        "getter": "GetAmdExplicitVertexParameter",
    },
    {
        "file": "extinst.nonsemantic.debugprintf.grammar.json",
        "prefix": "NonSemanticDebugPrintf",
        "getter": "GetNonSemanticDebugPrintf",
    },
]


def method_to_opname(name):
    """Return the grammar opname for a Module method name, or None if it does
    not correspond to a single SPIR-V opcode in the core grammar."""
    if name.startswith("Op"):
        return name
    # Type-Declaration / Constant-Creation / SpecConstant* drop the "Op"
    # prefix in sirit's API to read like factories.
    if name.startswith(("Type", "Constant", "SpecConstant")):
        return "Op" + name
    # Annotation ops drop "Op" too -- both the existing Decorate/MemberDecorate
    # and the newer DecorationGroup / GroupDecorate / GroupMemberDecorate.
    if name in ("Decorate", "MemberDecorate", "DecorationGroup",
                "GroupDecorate", "GroupMemberDecorate"):
        return "Op" + name
    # Debug metadata ops (Name / MemberName / String / Source* / NoLine /
    # ModuleProcessed) drop "Op".
    if name in ("Name", "MemberName", "String", "Source", "SourceContinued",
                "SourceExtension", "NoLine", "ModuleProcessed"):
        return "Op" + name
    return None


# Stream target by grammar `class` field. Anything not listed defaults to
# `code` (the catch-all instruction stream).
CLASS_TO_STREAM = {
    "Type-Declaration": "declarations",
    "Constant-Creation": "declarations",
    "Annotation": "annotations",
    "Debug": "debug",
}


def stream_for(opname, klass):
    if opname == "OpEntryPoint":
        return "entry_points"
    if opname in ("OpExecutionMode", "OpExecutionModeId"):
        return "execution_modes"
    if opname == "OpExtInstImport":
        return "ext_inst_imports"
    return CLASS_TO_STREAM.get(klass, "code")


# ---------------------------------------------------------------------------
# Header parser
# ---------------------------------------------------------------------------

def strip_block_comments(text):
    return re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)


def strip_line_comments(text):
    # Keep line breaks so line numbers don't shift unnecessarily.
    return re.sub(r"//[^\n]*", "", text)


def find_matching(text, start, open_ch, close_ch):
    """Return index just past the matching close_ch, given an open_ch at
    text[start]. Returns len(text) if unbalanced."""
    depth = 1
    i = start + 1
    while i < len(text) and depth > 0:
        c = text[i]
        if c == open_ch:
            depth += 1
        elif c == close_ch:
            depth -= 1
        i += 1
    return i


def strip_templates(text):
    """Remove every `template <...> [requires(...)] <decl-or-defn>` block."""
    out = []
    i = 0
    template_re = re.compile(r"\btemplate\b\s*<")
    while True:
        m = template_re.search(text, i)
        if not m:
            out.append(text[i:])
            break
        out.append(text[i:m.start()])
        # Skip past `template <...>`.
        j = find_matching(text, m.end() - 1, "<", ">")
        # Now scan forward to the end of the templated declaration / definition.
        depth_paren = 0
        k = j
        while k < len(text):
            c = text[k]
            if c == "(":
                depth_paren += 1
            elif c == ")":
                depth_paren -= 1
            elif c == ";" and depth_paren == 0:
                k += 1
                break
            elif c == "{" and depth_paren == 0:
                k = find_matching(text, k, "{", "}")
                break
            k += 1
        i = k
    return "".join(out)


def find_module_class_body(text):
    """Return the substring inside `class Module { ... };`."""
    m = re.search(r"\bclass\s+Module\b\s*\{", text)
    if not m:
        raise RuntimeError("class Module not found")
    body_end = find_matching(text, m.end() - 1, "{", "}")
    return text[m.end():body_end - 1]


# Match a non-template declaration ending in `;`:
#   <ret> <name>(<params>);
# We disallow `=` in params before the `(` and require a primitive-ish return
# type starting word (Id, void, std::vector<...>, spv::*).
DECL_RE = re.compile(
    r"""
    (?<![\w&:])                              # not preceded by a word-char
    (?P<ret>
        (?: Id | void | bool
          | std::vector<[\w\s,:<>]+>
        )
        (?:\s*[\w<>:,\s\*&]*?)?              # qualifiers/template args
    )
    \s+(?P<name>\w+)\s*                      # method name
    \((?P<params>[^)]*)\)\s*                 # (params)
    ;
    """,
    re.VERBOSE,
)


def parse_param(p):
    """Parse a single C++ parameter declaration. Returns (type, name).
    Default values are stripped (we only need the name for the chain)."""
    p = p.strip()
    if not p:
        return None
    # Drop default value.
    if "=" in p:
        p = p[:p.index("=")].rstrip()
    # The parameter name is the last identifier before any trailing array/ref.
    # We split on the LAST whitespace that is not inside angle/round brackets.
    depth = 0
    split_at = -1
    for i, ch in enumerate(p):
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth -= 1
        elif ch.isspace() and depth == 0:
            split_at = i
    if split_at == -1:
        # Single token; treat as type with no name.
        return (p, "")
    type_part = p[:split_at].rstrip()
    name_part = p[split_at + 1:].strip()
    # Strip leading & or * if name accidentally captured them.
    while name_part and name_part[0] in "&*":
        type_part += name_part[0]
        name_part = name_part[1:]
    return (type_part, name_part)


def split_top_level(s, sep=","):
    """Split `s` on `sep` at depth 0 (ignoring nesting in <...> and (...))."""
    out = []
    depth = 0
    last = 0
    for i, ch in enumerate(s):
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth -= 1
        elif ch == sep and depth == 0:
            out.append(s[last:i])
            last = i + 1
    out.append(s[last:])
    return out


def parse_methods(header_text):
    text = strip_block_comments(header_text)
    text = strip_line_comments(text)
    text = find_module_class_body(text)
    text = strip_templates(text)

    methods = []
    for m in DECL_RE.finditer(text):
        ret = m.group("ret").strip()
        name = m.group("name")
        params_raw = m.group("params").strip()
        params = []
        if params_raw:
            for chunk in split_top_level(params_raw):
                parsed = parse_param(chunk)
                if parsed is None:
                    continue
                params.append(parsed)
        methods.append({"ret": ret, "name": name, "params": params})
    return methods


# ---------------------------------------------------------------------------
# Reserve-size estimator
# ---------------------------------------------------------------------------

# Word size for an operand of a given grammar `kind` -- as a Python expression
# template that may reference the C++ parameter name.
#
# For optional<T> and span<const T>, the runtime cost is encoded against the
# parameter name (e.g. "indexes.size()", "memory_access.has_value() ? 1 : 0").
def operand_size_term(kind, quantifier, param_name):
    if quantifier == "*":
        # Variadic: <name>.size() words, one per element.
        return f"{param_name}.size()"
    if quantifier == "?":
        # Optional: 1 word if present, 0 otherwise.
        # We assume optional<T> for a fixed-size T (the common case).
        return f"({param_name}.has_value() ? 1 : 0)"
    if kind == "LiteralString":
        return f"WordsInString({param_name})"
    # All other fixed-size kinds occupy 1 word.
    # Note: LiteralContextDependentNumber may take 2 words for u64/double; the
    # existing constant.cpp reserves a conservative 2 for the Constant case.
    if kind == "LiteralContextDependentNumber":
        return "2"
    return "1"


def reserve_expr(operands):
    """Build a Reserve(...) expression from grammar operand kinds + names."""
    base = 1  # the opcode word itself
    base += sum(1 for op in operands if op.get("kind") in ("IdResultType", "IdResult"))

    extra_terms = []
    for op in operands:
        kind = op.get("kind")
        if kind in ("IdResultType", "IdResult"):
            continue
        param_name = op.get("_param_name")
        if param_name is None:
            # Optional grammar operand not exposed by the public API; the
            # encoded instruction won't include it, so it costs zero words.
            continue
        term = operand_size_term(kind, op.get("quantifier", ""), param_name)
        extra_terms.append(term)

    if not extra_terms:
        return str(base)
    # Sum constants where possible to keep the expression tidy.
    const_total = base
    runtime_terms = []
    for t in extra_terms:
        if t == "1":
            const_total += 1
        elif t == "2":
            const_total += 2
        else:
            runtime_terms.append(t)
    parts = [str(const_total)] + runtime_terms
    return " + ".join(parts)


# ---------------------------------------------------------------------------
# Body generator
# ---------------------------------------------------------------------------

def head_form(operands):
    has_rt = bool(operands) and operands[0].get("kind") == "IdResultType"
    has_id = any(op.get("kind") == "IdResult" for op in operands)
    if has_rt and has_id:
        return "opid_rt"
    if has_id:
        return "opid_no_rt"
    return "raw"


def assign_param_names(operands, c_params):
    """Walk `operands` (skipping IdResultType/IdResult) and pair each remaining
    operand with the next C++ parameter. Sets `op['_param_name']` in place.

    Returns (chain_param_names, error_message_or_None).
    """
    # If operand list expects IdResultType, the first C++ param is `result_type`.
    has_rt = bool(operands) and operands[0].get("kind") == "IdResultType"
    if has_rt:
        if not c_params or c_params[0][1] != "result_type":
            return [], (
                f"first C++ parameter is not `result_type` "
                f"(got {c_params[0][1] if c_params else 'no params'!r})"
            )
        c_iter = iter(c_params[1:])
    else:
        c_iter = iter(c_params)

    chain_names = []
    c_remaining = list(c_iter)
    c_idx = 0
    for op in operands:
        if op.get("kind") in ("IdResultType", "IdResult"):
            continue
        if c_idx >= len(c_remaining):
            # No more C++ params. Allowed only for optional grammar operands
            # that the public API has chosen not to expose.
            if op.get("quantifier") == "?":
                continue
            return chain_names, (
                f"ran out of C++ params while binding required grammar "
                f"operand {op.get('kind')}"
            )
        _, pname = c_remaining[c_idx]
        c_idx += 1
        op["_param_name"] = pname
        chain_names.append(pname)
    leftover = c_remaining[c_idx:]

    # Allow leftover C++ params only if the last grammar operand is a "trailing
    # parameterized" kind (mask / value-enum-with-params), which the C++ side
    # carries via an extra `<< span` token.
    if leftover:
        # Heuristic: if the last grammar operand kind is in PARAMETERIZED_KINDS,
        # accept ONE leftover param. The C++ side passes a `std::span<const T>`
        # carrying the per-mask-bit (or per-decoration) word group; treat it
        # like a synthetic variadic operand so Reserve() accounts for its size.
        if len(leftover) == 1:
            leftover_name = leftover[0][1]
            chain_names.append(leftover_name)
            operands.append({
                "kind": "_TrailingSpan",
                "quantifier": "*",
                "_param_name": leftover_name,
            })
        else:
            return chain_names, (
                f"{len(leftover)} extra C++ params after binding all operands"
            )
    return chain_names, None


def render_method(method, grammar, extinst_lookup):
    name = method["name"]
    if name in SKIP:
        return None, "in skip-list"

    # Method-name override (e.g. OpAtomicFMax -> OpAtomicFMaxEXT) wins.
    if name in OPNAME_OVERRIDES:
        opname = OPNAME_OVERRIDES[name]
    else:
        opname = method_to_opname(name)

    if opname is None:
        return None, f"no opname mapping for method '{name}'"

    # Extended-instruction wrapper path: method is an OpFoo whose `Foo` lives
    # in one of our known extinst grammars but not in core.
    if opname not in grammar and name.startswith("Op"):
        ext_key = name[2:]
        if ext_key in extinst_lookup:
            return render_extinst_wrapper(method, extinst_lookup[ext_key])

    if opname not in grammar:
        return None, f"opname {opname} not in grammar"

    inst = grammar[opname]
    klass = inst.get("class", "")
    operands = [dict(op) for op in inst.get("operands", [])]  # shallow copies
    form = head_form(operands)
    stream = stream_for(opname, klass)

    chain_names, err = assign_param_names(operands, method["params"])
    if err:
        return None, f"signature/grammar mismatch: {err}"

    reserve = reserve_expr(operands)
    chain = "".join(f" << {n}" for n in chain_names)

    if form == "opid_rt":
        head = f"OpId{{spv::Op::{opname}, result_type}}"
    elif form == "opid_no_rt":
        head = f"OpId{{spv::Op::{opname}}}"
    else:
        head = f"spv::Op::{opname}"

    # Build the parameter list from the original method.
    params_src = ", ".join(_format_param(p) for p in method["params"])
    ret = method["ret"]

    if ret == "void":
        return_kw = ""
    else:
        return_kw = "return "

    body = (
        f"{ret} Module::{name}({params_src}) {{\n"
        f"    {stream}->Reserve({reserve});\n"
        f"    {return_kw}*{stream} << {head}{chain} << EndOp{{}};\n"
        f"}}\n"
    )
    return body, None


def _format_param(p):
    type_part, name = p
    # Re-emit as "Type name". Don't re-add default values: those are in the
    # header declaration, not in the .cpp definition.
    return f"{type_part} {name}".strip()


def render_extinst_wrapper(method, ext_entry):
    """Render an `OpFoo` body that delegates to OpExtInst with the right
    set + opcode constant. `ext_entry` carries the extinst metadata."""
    name = method["name"]
    params = method["params"]
    if not params or params[0][1] != "result_type":
        return None, (
            f"extinst wrapper {name}: first parameter must be `result_type` "
            f"(got {params[0][1] if params else 'none'!r})"
        )
    arg_names = [p[1] for p in params[1:]]
    args_src = (", " + ", ".join(arg_names)) if arg_names else ""
    constant = ext_entry["prefix"] + ext_entry["opname"]
    getter = ext_entry["getter"]
    params_src = ", ".join(_format_param(p) for p in params)
    body = (
        f"{method['ret']} Module::{name}({params_src}) {{\n"
        f"    return OpExtInst(result_type, {getter}(), {constant}{args_src});\n"
        f"}}\n"
    )
    return body, None


# ---------------------------------------------------------------------------
# Driver
# ---------------------------------------------------------------------------

HEADER_BLURB = """\
/* This file is auto-generated by tools/generate_instructions.py.
 * Do not edit by hand. Regenerate after changes to the public header
 * include/sirit/sirit.h or to the SPIR-V grammar files
 * (externals/SPIRV-Headers/include/spirv/unified1/spirv.core.grammar.json
 * and the bundled extinst.*.grammar.json files).
 */

#include <spirv/unified1/AMD_gcn_shader.h>
#include <spirv/unified1/AMD_shader_explicit_vertex_parameter.h>
#include <spirv/unified1/AMD_shader_trinary_minmax.h>
#include <spirv/unified1/GLSL.std.450.h>
#include <spirv/unified1/NonSemanticDebugPrintf.h>

#include "sirit/sirit.h"

#include "stream.h"

namespace Sirit {

"""

FOOTER = "} // namespace Sirit\n"


def load_extinst_lookup():
    """Returns a dict mapping extinst opname (without "Op" prefix) to a record
    {prefix, getter, opname} suitable for render_extinst_wrapper."""
    base_dir = (
        ROOT / "externals" / "SPIRV-Headers" / "include" / "spirv" / "unified1"
    )
    lookup = {}
    for entry in EXTINST_SETS:
        path = base_dir / entry["file"]
        if not path.exists():
            continue
        data = json.loads(path.read_text(encoding="utf-8"))
        for inst in data.get("instructions", []):
            opname = inst["opname"]
            lookup[opname] = {
                "prefix": entry["prefix"],
                "getter": entry["getter"],
                "opname": opname,
            }
    return lookup


def main():
    grammar_data = json.loads(GRAMMAR_PATH.read_text(encoding="utf-8"))
    grammar = {inst["opname"]: inst for inst in grammar_data["instructions"]}
    extinst_lookup = load_extinst_lookup()

    header_text = HEADER_PATH.read_text(encoding="utf-8")
    methods = parse_methods(header_text)

    bodies = []
    skipped = []
    errored = []

    for m in methods:
        body, err = render_method(m, grammar, extinst_lookup)
        if body is not None:
            bodies.append(body)
        elif err and ("skip-list" in err or "no opname mapping" in err):
            skipped.append((m["name"], err))
        else:
            errored.append((m["name"], err))

    OUTPUT_PATH.write_text(
        HEADER_BLURB + "\n".join(bodies) + "\n" + FOOTER, encoding="utf-8"
    )

    print(f"Wrote {OUTPUT_PATH.relative_to(ROOT).as_posix()}")
    print(f"  generated: {len(bodies)} method(s)")
    print(f"  skipped:   {len(skipped)} method(s)")
    print(f"  errors:    {len(errored)} method(s)")
    if errored:
        print()
        print("Errors (need manual review or skip-list entry):")
        for name, err in errored:
            print(f"  {name}: {err}")
    return 1 if errored else 0


if __name__ == "__main__":
    sys.exit(main())
