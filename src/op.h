/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#pragma once

#include <optional>
#include "common_types.h"
#include "operand.h"
#include "sirit/sirit.h"
#include "stream.h"

namespace Sirit {

class Op final : public Operand {
public:
    explicit Op(spv::Op opcode, std::optional<u32> id = {}, Id result_type = nullptr);
    ~Op() override;

    void Fetch(Stream& stream) const override;

    u16 GetWordCount() const noexcept override;

    bool operator==(const Operand& other) const noexcept override;

    void Write(Stream& stream) const;

    void Sink(std::unique_ptr<Operand> operand);

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
