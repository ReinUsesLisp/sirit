/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "insts.h"
#include "sirit/sirit.h"
#include <cassert>

namespace Sirit {

Id Module::OpVariable(Id result_type, spv::StorageClass storage_class,
                       Id initializer) {
    auto op{new Op(spv::Op::OpVariable, bound++, result_type)};
    AddEnum(op, storage_class);
    if (initializer) {
        op->Add(initializer);
    }
    return AddCode(op);
}

Id Module::OpLoad(Id result_type, Id pointer,
                   std::optional<spv::MemoryAccessMask> memory_access) {
    auto op{new Op(spv::Op::OpLoad, bound++, result_type)};
    op->Add(pointer);
    if (memory_access) {
        AddEnum(op, *memory_access);
    }
    return AddCode(op);
}

Id Module::OpStore(Id pointer, Id object,
                    std::optional<spv::MemoryAccessMask> memory_access) {
    auto op{new Op(spv::Op::OpStore)};
    op->Add(pointer);
    op->Add(object);
    if (memory_access) {
        AddEnum(op, *memory_access);
    }
    return AddCode(op);
}

Id Module::OpAccessChain(Id result_type, Id base,
                          const std::vector<Id>& indexes) {
    assert(indexes.size() > 0);
    auto op{new Op(spv::Op::OpAccessChain, bound++, result_type)};
    op->Add(base);
    op->Add(indexes);
    return AddCode(op);
}

Id Module::OpCompositeInsert(Id result_type, Id object, Id composite,
                              const std::vector<Literal>& indexes) {
    auto op{new Op(spv::Op::OpCompositeInsert, bound++, result_type)};
    op->Add(object);
    op->Add(composite);
    op->Add(indexes);
    return AddCode(op);
}

} // namespace Sirit
