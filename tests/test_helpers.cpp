/* Implementations of validation / disassembly / cross-compile helpers.
 *
 * SPIRV-Tools provides a stable C and C++ API; we use the C++ wrapper from
 * <spirv-tools/libspirv.hpp>. SPIRV-Cross is plain C++ headers.
 */

#include "test_helpers.h"

#include <algorithm>
#include <cstring>
#include <exception>
#include <set>
#include <sstream>

#include <spirv-tools/libspirv.hpp>

namespace sirit_tests {

int g_total = 0;
int g_failures = 0;
const char* g_current_test = "";

namespace {

const char* LevelName(spv_message_level_t level) {
    switch (level) {
    case SPV_MSG_FATAL:          return "fatal";
    case SPV_MSG_INTERNAL_ERROR: return "internal";
    case SPV_MSG_ERROR:          return "error";
    case SPV_MSG_WARNING:        return "warning";
    case SPV_MSG_INFO:           return "info";
    case SPV_MSG_DEBUG:          return "debug";
    }
    return "?";
}

}  // namespace

ValidationResult Validate(std::span<const std::uint32_t> words, spv_target_env env) {
    ValidationResult result;
    spvtools::SpirvTools tools(env);
    std::ostringstream messages;
    tools.SetMessageConsumer([&messages](spv_message_level_t level, const char* /*src*/,
                                         const spv_position_t& pos, const char* msg) {
        messages << "  spirv-val [" << LevelName(level) << "] @"
                 << pos.line << ":" << pos.column << ": " << msg << '\n';
    });
    result.ok = tools.Validate(words.data(), words.size());
    if (!result.ok) {
        // Append a disassembly so failures are debuggable without rerunning.
        std::string dis;
        tools.Disassemble(words.data(), words.size(), &dis,
                          SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES |
                              SPV_BINARY_TO_TEXT_OPTION_INDENT |
                              SPV_BINARY_TO_TEXT_OPTION_COMMENT);
        messages << "\n--- module disassembly ---\n" << dis;
    }
    result.messages = messages.str();
    return result;
}

std::string Disassemble(std::span<const std::uint32_t> words, spv_target_env env) {
    spvtools::SpirvTools tools(env);
    std::string out;
    tools.Disassemble(words.data(), words.size(), &out,
                      SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES |
                          SPV_BINARY_TO_TEXT_OPTION_INDENT);
    return out;
}

AssembleResult AssembleText(const std::string& text, spv_target_env env) {
    AssembleResult result;
    spvtools::SpirvTools tools(env);
    std::ostringstream messages;
    tools.SetMessageConsumer([&messages](spv_message_level_t level, const char* /*src*/,
                                         const spv_position_t& pos, const char* msg) {
        messages << "  spirv-as [" << LevelName(level) << "] @"
                 << pos.line << ":" << pos.column << ": " << msg << '\n';
    });
    result.ok = tools.Assemble(text, &result.words);
    if (!result.ok) {
        result.error = messages.str();
    }
    return result;
}


// === Module structural summary =============================================

namespace {

struct ParsedInst {
    spv::Op opcode{};
    std::uint32_t word_count{};
    const std::uint32_t* words = nullptr;
};

std::vector<ParsedInst> ParseInstructionsLocal(std::span<const std::uint32_t> words) {
    std::vector<ParsedInst> out;
    if (words.size() < 5) {
        return out;
    }
    std::size_t i = 5;
    while (i < words.size()) {
        const std::uint32_t header = words[i];
        const std::uint32_t op = header & 0xffffu;
        const std::uint32_t wc = header >> 16;
        if (wc == 0 || i + wc > words.size()) {
            break;
        }
        out.push_back({static_cast<spv::Op>(op), wc, &words[i]});
        i += wc;
    }
    return out;
}

std::string ReadString(const std::uint32_t* words, std::size_t max_words) {
    std::string out;
    out.reserve(max_words * 4);
    for (std::size_t i = 0; i < max_words; ++i) {
        const std::uint32_t w = words[i];
        for (int b = 0; b < 4; ++b) {
            const char c = static_cast<char>((w >> (b * 8)) & 0xff);
            if (c == '\0') {
                return out;
            }
            out.push_back(c);
        }
    }
    return out;
}

std::size_t WordsForString(const std::uint32_t* words, std::size_t available) {
    // Number of u32 words occupied by a packed null-terminated string,
    // counting the word that contains the terminator.
    for (std::size_t i = 0; i < available; ++i) {
        const std::uint32_t w = words[i];
        for (int b = 0; b < 4; ++b) {
            if (((w >> (b * 8)) & 0xff) == 0) {
                return i + 1;
            }
        }
    }
    return available;
}

}  // namespace

ModuleSummary Summarize(std::span<const std::uint32_t> words) {
    ModuleSummary sum;
    std::set<spv::Capability> caps;
    std::set<std::string> exts;
    bool memory_model_seen = false;

    for (const auto& inst : ParseInstructionsLocal(words)) {
        sum.opcode_counts[inst.opcode]++;
        switch (inst.opcode) {
        case spv::Op::OpCapability:
            if (inst.word_count >= 2) {
                caps.insert(static_cast<spv::Capability>(inst.words[1]));
            }
            break;
        case spv::Op::OpExtension:
            if (inst.word_count >= 2) {
                exts.insert(ReadString(&inst.words[1], inst.word_count - 1));
            }
            break;
        case spv::Op::OpMemoryModel:
            if (inst.word_count >= 3 && !memory_model_seen) {
                sum.addressing_model = static_cast<spv::AddressingModel>(inst.words[1]);
                sum.memory_model = static_cast<spv::MemoryModel>(inst.words[2]);
                memory_model_seen = true;
            }
            break;
        case spv::Op::OpEntryPoint: {
            if (inst.word_count >= 4) {
                EntryPointSummary ep;
                ep.model = static_cast<spv::ExecutionModel>(inst.words[1]);
                // words[2] = entry-point function id (renumberable; ignore)
                // words[3..] = name (LiteralString) followed by interface ids
                const std::uint32_t* name_words = &inst.words[3];
                const std::size_t avail = inst.word_count - 3;
                const std::size_t name_word_count = WordsForString(name_words, avail);
                ep.name = ReadString(name_words, name_word_count);
                ep.interface_count = avail - name_word_count;
                sum.entry_points.push_back(ep);
            }
            break;
        }
        default:
            break;
        }
    }

    sum.capabilities.assign(caps.begin(), caps.end());
    sum.extensions.assign(exts.begin(), exts.end());
    std::sort(sum.entry_points.begin(), sum.entry_points.end(),
              [](const EntryPointSummary& a, const EntryPointSummary& b) {
                  if (a.model != b.model) {
                      return static_cast<std::uint32_t>(a.model) <
                             static_cast<std::uint32_t>(b.model);
                  }
                  return a.name < b.name;
              });
    return sum;
}

std::string SummaryDiff(const ModuleSummary& expected, const ModuleSummary& actual) {
    if (expected == actual) {
        return {};
    }
    std::ostringstream out;
    out << "ModuleSummary mismatch:\n";

    if (expected.capabilities != actual.capabilities) {
        out << "  capabilities differ: expected={";
        for (auto c : expected.capabilities) out << static_cast<std::uint32_t>(c) << ",";
        out << "} actual={";
        for (auto c : actual.capabilities) out << static_cast<std::uint32_t>(c) << ",";
        out << "}\n";
    }
    if (expected.extensions != actual.extensions) {
        out << "  extensions differ: expected={";
        for (const auto& e : expected.extensions) out << e << ",";
        out << "} actual={";
        for (const auto& e : actual.extensions) out << e << ",";
        out << "}\n";
    }
    if (expected.addressing_model != actual.addressing_model) {
        out << "  addressing_model: expected=" << static_cast<int>(expected.addressing_model)
            << " actual=" << static_cast<int>(actual.addressing_model) << '\n';
    }
    if (expected.memory_model != actual.memory_model) {
        out << "  memory_model: expected=" << static_cast<int>(expected.memory_model)
            << " actual=" << static_cast<int>(actual.memory_model) << '\n';
    }
    if (expected.entry_points != actual.entry_points) {
        out << "  entry points differ:\n";
        out << "    expected: ";
        for (const auto& ep : expected.entry_points) {
            out << "(model=" << static_cast<int>(ep.model) << ", name=\"" << ep.name
                << "\", interfaces=" << ep.interface_count << ") ";
        }
        out << "\n    actual:   ";
        for (const auto& ep : actual.entry_points) {
            out << "(model=" << static_cast<int>(ep.model) << ", name=\"" << ep.name
                << "\", interfaces=" << ep.interface_count << ") ";
        }
        out << '\n';
    }
    if (expected.opcode_counts != actual.opcode_counts) {
        out << "  opcode counts differ:\n";
        std::set<spv::Op> all_ops;
        for (const auto& [op, _] : expected.opcode_counts) all_ops.insert(op);
        for (const auto& [op, _] : actual.opcode_counts) all_ops.insert(op);
        for (auto op : all_ops) {
            const auto e = expected.opcode_counts.contains(op) ? expected.opcode_counts.at(op) : 0;
            const auto a = actual.opcode_counts.contains(op) ? actual.opcode_counts.at(op) : 0;
            if (e != a) {
                out << "    op=" << static_cast<std::uint32_t>(op) << ": expected=" << e
                    << " actual=" << a << '\n';
            }
        }
    }
    return out.str();
}


}  // namespace sirit_tests
