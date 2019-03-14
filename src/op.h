/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
 */

#pragma once

#include <optional>
#include "common_types.h"
#include "operand.h"
#include "sirit/sirit.h"
#include "stream.h"

namespace Sirit {

class Op : public Operand {
public:
    explicit Op(spv::Op opcode, std::optional<u32> id = {}, Id result_type = nullptr);
    ~Op() override;

    void Fetch(Stream& stream) const override;
    u16 GetWordCount() const override;

    bool operator==(const Operand& other) const override;

    void Write(Stream& stream) const;

    void Sink(Operand* operand);

    void Sink(const std::vector<Operand*>& operands_);

    void Add(const Literal& literal);

    void Add(const std::vector<Literal>& literals);

    void Add(const Operand* operand);

    void Add(u32 integer);

    void Add(std::string string);

    void Add(const std::vector<Id>& ids);

private:
    u16 WordCount() const;

    spv::Op opcode;

    Id result_type;

    std::optional<u32> id;

    std::vector<const Operand*> operands;

    std::vector<std::unique_ptr<Operand>> operand_store;
};

} // namespace Sirit
