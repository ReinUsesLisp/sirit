/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2 or any later version.
 */

#include "stream.h"

namespace Sirit {

Stream::Stream(std::vector<u8>& bytes_)
    : bytes(bytes_) {}

Stream::~Stream() = default;

void Stream::Write(std::string string) {
    std::size_t size{string.size()};
    u8* data{reinterpret_cast<u8*>(string.data())};
    for (std::size_t i{}; i < size; i++) {
        Write(data[i]);
    }
    for (std::size_t i{}; i < 4 - size % 4; i++) {
        Write(static_cast<u8>(0));
    }
}

void Stream::Write(u64 value) {
    u32* mem{reinterpret_cast<u32*>(&value)};
    Write(mem[0]);
    Write(mem[1]);
}

void Stream::Write(u32 value) {
    u16* mem{reinterpret_cast<u16*>(&value)};
    Write(mem[0]);
    Write(mem[1]);
}

void Stream::Write(u16 value) {
    u8* mem{reinterpret_cast<u8*>(&value)};
    Write(mem[0]);
    Write(mem[1]);
}

void Stream::Write(u8 value) {
    bytes.push_back(value);
}

} // namespace Sirit
