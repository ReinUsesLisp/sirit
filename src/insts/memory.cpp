/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "insts.h"
#include "sirit/sirit.h"
#include <cassert>

namespace Sirit {

Ref Module::OpVariable(Ref result_type, spv::StorageClass storage_class,
                       Ref initializer) {
    auto op{new Op(spv::Op::OpVariable, bound++, result_type)};
    AddEnum(op, storage_class);
    if (initializer) {
        op->Add(initializer);
    }
    return AddCode(op);
}

Ref Module::OpLoad(Ref result_type, Ref pointer,
                   std::optional<spv::MemoryAccessMask> memory_access) {
    auto op{new Op(spv::Op::OpLoad, bound++, result_type)};
    op->Add(pointer);
    if (memory_access) {
        AddEnum(op, *memory_access);
    }
    return AddCode(op);
}

Ref Module::OpStore(Ref pointer, Ref object,
                    std::optional<spv::MemoryAccessMask> memory_access) {
    auto op{new Op(spv::Op::OpStore)};
    op->Add(pointer);
    op->Add(object);
    if (memory_access) {
        AddEnum(op, *memory_access);
    }
    return AddCode(op);
}

Ref Module::OpAccessChain(Ref result_type, Ref base,
                          const std::vector<Ref>& indexes) {
    assert(indexes.size() > 0);
    auto op{new Op(spv::Op::OpAccessChain, bound++, result_type)};
    op->Add(base);
    op->Add(indexes);
    return AddCode(op);
}

Ref Module::OpCompositeInsert(Ref result_type, Ref object, Ref composite,
                              const std::vector<Literal>& indexes) {
    auto op{new Op(spv::Op::OpCompositeInsert, bound++, result_type)};
    op->Add(object);
    op->Add(composite);
    op->Add(indexes);
    return AddCode(op);
}

} // namespace Sirit
