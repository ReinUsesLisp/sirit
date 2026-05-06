"""Validate src/instructions/*.cpp emitters against spirv.core.grammar.json.

For each method body that emits a single chained SPIR-V instruction
(`*stream << HEAD << ... << EndOp{}`), this script verifies:

  1. opcode-name match  -- if the method name starts with `Op`, the emitted
     opcode name must equal the method name. (Catches copy-paste bugs like
     `OpImageSparseRead` accidentally emitting `OpImageSparseTexelsResident`.)

  2. head-form match    -- the OpId{} variant used must agree with the
     grammar's IdResultType / IdResult presence:
        * grammar has IdResultType + IdResult -> code must use OpId{op, rt}
        * grammar has IdResult only           -> code must use OpId{op}
                                                 (the OpId path always emits
                                                 a result-id; correct for
                                                 Type-Declaration ops)
        * grammar has neither                 -> code must use raw
                                                 `spv::Op::Op<X>` (the OpId
                                                 path would write a spurious
                                                 result-id; this catches the
                                                 OpAtomicStore bug type)

  3. operand-count match -- the number of `<<`-separated operand expressions
     between HEAD and EndOp must lie in [required, total + parameterized],
     where:
        required = count of grammar operands without `?` quantifier
                   (excluding IdResultType / IdResult)
        total    = count of all grammar operands
                   (excluding IdResultType / IdResult)
        parameterized = 1 if the last grammar operand is a BitEnum mask or a
                   ValueEnum whose enumerants carry per-value parameters
                   (Decoration, ExecutionMode, ImageOperands, LoopControl,
                   MemoryAccess, ...). C++ producers add one extra `<<` token
                   to carry the per-value variadic word group.

Methods declared inside `#define` macro bodies (lines ending with `\\`) are
skipped: the placeholder method names there don't correspond to grammar
opnames. Multi-statement emit patterns (OpBranchConditional, OpSwitch,
DeferredOpPhi etc.) are reported as skipped for manual review.

Exit code: 0 if no issues, 1 otherwise.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GRAMMAR_PATH = (
    ROOT / "externals" / "SPIRV-Headers" / "include" / "spirv" / "unified1"
    / "spirv.core.grammar.json"
)
INST_DIR = ROOT / "src" / "instructions"


def load_grammar():
    with GRAMMAR_PATH.open(encoding="utf-8") as f:
        data = json.load(f)
    by_opname = {inst["opname"]: inst for inst in data["instructions"]}
    parameterized = compute_parameterized_kinds(data)
    return by_opname, parameterized


def compute_parameterized_kinds(data):
    """Operand kinds whose values may consume additional words from the wire,
    which in this codebase translates to one extra `<<` token in the chain."""
    out = set()
    for kind in data.get("operand_kinds", []):
        category = kind.get("category")
        name = kind.get("kind")
        if category == "BitEnum":
            out.add(name)
        elif category == "ValueEnum":
            for enumerant in kind.get("enumerants", []):
                if enumerant.get("parameters"):
                    out.add(name)
                    break
    return out


def expected_form(operands):
    has_rt = bool(operands) and operands[0].get("kind") == "IdResultType"
    has_id = any(op.get("kind") == "IdResult" for op in operands)
    if has_rt and has_id:
        return "opid_rt"
    if has_id and not has_rt:
        return "opid_no_rt"
    if not has_id and not has_rt:
        return "raw"
    return "unknown"


def remaining_operands(operands):
    return [
        op for op in operands
        if op.get("kind") not in ("IdResultType", "IdResult")
    ]


METHOD_RE = re.compile(
    r"""
    (?P<ret>(?:\w[\w<>:&\s,*]*?))            # return type
    \s+Module::(?P<name>\w+)\s*              # Module::Name
    \((?P<params>[^)]*)\)\s*\{               # (params) {
    (?P<body>.*?)                            # body (non-greedy)
    \n\}                                     # closing } at column 0
    """,
    re.DOTALL | re.VERBOSE,
)

EMIT_RE = re.compile(
    r"""
    \*\s*\w+\s*                              # *target
    <<\s*
    (?P<head>OpId\s*\{[^}]+\}|spv::Op::Op\w+)
    \s*(?P<rest>(?:<<\s*[^;]+?)?)            # << op << op ...
    <<\s*EndOp\s*\{\s*\}                     # << EndOp{}
    """,
    re.DOTALL | re.VERBOSE,
)

OPID_RT_RE = re.compile(
    r"OpId\s*\{\s*spv::Op::Op(?P<op>\w+)\s*,\s*[\w_]+\s*\}"
)
OPID_NO_RT_RE = re.compile(r"OpId\s*\{\s*spv::Op::Op(?P<op>\w+)\s*\}")
RAW_RE = re.compile(r"spv::Op::Op(?P<op>\w+)")


def classify_head(head):
    s = head.strip()
    m = OPID_RT_RE.fullmatch(s)
    if m:
        return "opid_rt", m.group("op")
    m = OPID_NO_RT_RE.fullmatch(s)
    if m:
        return "opid_no_rt", m.group("op")
    m = RAW_RE.fullmatch(s)
    if m:
        return "raw", m.group("op")
    return None, None


def split_operands(rest):
    rest = rest.strip()
    if not rest:
        return []
    flat = re.sub(r"\s+", " ", rest)
    parts = re.split(r"<<", flat)
    return [p.strip() for p in parts if p.strip()]


def line_of_offset(text, offset):
    return text.count("\n", 0, offset) + 1


def is_in_macro(text, name_offset):
    """Returns True if the line containing `name_offset` ends with a backslash
    line-continuation (i.e. the method declaration is inside a #define)."""
    line_start = text.rfind("\n", 0, name_offset) + 1
    line_end = text.find("\n", name_offset)
    if line_end == -1:
        line_end = len(text)
    return text[line_start:line_end].rstrip().endswith("\\")


# Methods we expect not to emit (helpers / wrappers / no-ops).
NON_EMITTING = {
    "OpLabel",                          # only allocates an id
    "OpDemoteToHelperInvocationEXT",    # alias-wrapper around OpDemoteToHelperInvocation
}


# Method name -> grammar opcode name overrides for cases where the public API
# intentionally drops the spec suffix (e.g. EXT) for ergonomics. The generator
# in generate_instructions.py uses the same map; keep them in sync.
OPNAME_OVERRIDES = {
    "OpAtomicFMax": "OpAtomicFMaxEXT",
    "OpAtomicFMin": "OpAtomicFMinEXT",
}


def validate_file(path, grammar, parameterized):
    text = path.read_text(encoding="utf-8")
    issues = []
    skipped = []

    for mm in METHOD_RE.finditer(text):
        method_name = mm.group("name")
        body = mm.group("body")
        body_offset = mm.start("body")
        method_line = line_of_offset(text, mm.start("name"))

        if is_in_macro(text, mm.start("name")):
            continue

        emits = list(EMIT_RE.finditer(body))

        if len(emits) != 1:
            if method_name in NON_EMITTING:
                continue
            if not re.search(r"spv::Op::Op\w+", body):
                continue
            reason = "no single-statement emit chain found"
            if len(emits) > 1:
                reason = f"{len(emits)} emit chains found"
            skipped.append((method_line, method_name, reason))
            continue

        em = emits[0]
        head = em.group("head")
        rest = em.group("rest") or ""
        chain_tokens = split_operands(rest)
        emit_line = line_of_offset(text, body_offset + em.start())

        form, op = classify_head(head)
        if form is None:
            skipped.append((emit_line, method_name, f"unparseable head: {head!r}"))
            continue

        opname = "Op" + op

        # Check 1: method-name vs emitted-opcode mismatch.
        # Allow the documented overrides (e.g. OpAtomicFMax -> OpAtomicFMaxEXT).
        expected_opname = OPNAME_OVERRIDES.get(method_name, method_name)
        if method_name.startswith("Op") and expected_opname != opname:
            issues.append((
                emit_line, method_name,
                f"emits {opname} but method is named {method_name}"
                + (f" (expected {expected_opname})" if expected_opname != method_name else ""),
            ))
            continue

        if opname not in grammar:
            issues.append((
                emit_line, method_name,
                f"emits unknown opcode {opname} (not in grammar)",
            ))
            continue

        operands = grammar[opname].get("operands", [])
        expected = expected_form(operands)

        # Check 2: head-form vs grammar
        if form != expected:
            kinds = [op_.get("kind") for op_ in operands]
            issues.append((
                emit_line, method_name,
                f"emits {opname} using head form '{form}' but grammar "
                f"expects '{expected}' (operands: {kinds})",
            ))
            continue

        # Check 3: operand count
        remaining = remaining_operands(operands)
        required = sum(1 for op in remaining if op.get("quantifier") != "?")
        total = len(remaining)
        last_kind = remaining[-1].get("kind") if remaining else None
        max_extra = 1 if last_kind in parameterized else 0
        max_count = total + max_extra
        actual = len(chain_tokens)
        if not (required <= actual <= max_count):
            kinds_q = [
                f"{op_.get('kind')}{op_.get('quantifier','')}" for op_ in remaining
            ]
            issues.append((
                emit_line, method_name,
                f"emits {opname} with {actual} operand expression(s); "
                f"grammar expects {required}..{max_count} (operands: {kinds_q})",
            ))
            continue

    return issues, skipped


def main():
    grammar, parameterized = load_grammar()
    all_issues = []
    all_skipped = []

    for cpp in sorted(INST_DIR.glob("*.cpp")):
        rel = cpp.relative_to(ROOT).as_posix()
        issues, skipped = validate_file(cpp, grammar, parameterized)
        for line, method, msg in issues:
            all_issues.append(f"{rel}:{line} Module::{method}\n  {msg}")
        for line, method, reason in skipped:
            all_skipped.append(f"{rel}:{line} Module::{method} ({reason})")

    if all_issues:
        print("Issues:")
        for issue in all_issues:
            print()
            print(issue)
        print()

    if all_skipped:
        print("Skipped (multi-statement emits or unparseable; manual review):")
        for s in all_skipped:
            print(f"  {s}")
        print()

    print(f"{len(all_issues)} issue(s); {len(all_skipped)} skipped.")
    return 1 if all_issues else 0


if __name__ == "__main__":
    sys.exit(main())
