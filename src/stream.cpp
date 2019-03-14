/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
 */

#include "stream.h"

namespace Sirit {

Stream::Stream(std::vector<u8>& bytes) : bytes(bytes) {}

Stream::~Stream() = default;

void Stream::Write(std::string string) {
    const auto size{string.size()};
    const auto data{reinterpret_cast<u8*>(string.data())};
    for (std::size_t i = 0; i < size; i++) {
        Write(data[i]);
    }
    for (std::size_t i = 0; i < 4 - size % 4; i++) {
        Write(static_cast<u8>(0));
    }
}

void Stream::Write(u64 value) {
    const auto* const mem = reinterpret_cast<const u8*>(&value);
    bytes.insert(bytes.end(), mem, mem + sizeof(u64));
}

void Stream::Write(u32 value) {
    const auto* const mem = reinterpret_cast<const u8*>(&value);
    bytes.insert(bytes.end(), mem, mem + sizeof(u32));
}

void Stream::Write(u16 value) {
    const auto* const mem{reinterpret_cast<const u8*>(&value)};
    bytes.insert(bytes.end(), mem, mem + sizeof(u16));
}

void Stream::Write(u8 value) {
    bytes.push_back(value);
}

} // namespace Sirit
