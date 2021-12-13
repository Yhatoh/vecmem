/**
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#include "vecmem/memory/identity_memory_resource.hpp"

#include "vecmem/memory/memory_resource.hpp"

namespace vecmem {
identity_memory_resource::identity_memory_resource(memory_resource &upstream)
    : m_upstream(upstream) {}

void *identity_memory_resource::do_allocate(std::size_t size,
                                            std::size_t align) {
    /*
     * By definition, this just forwards the allocation upstream.
     */
    return m_upstream.allocate(size, align);
}

void identity_memory_resource::do_deallocate(void *ptr, std::size_t size,
                                             std::size_t align) {
    /*
     * The deallocation, like allocation, is a forwarding method.
     */
    m_upstream.deallocate(ptr, size, align);
}

bool identity_memory_resource::do_is_equal(
    const memory_resource &other) const noexcept {
    /*
     * These memory resources are equal if and only if they are the same
     * object.
     */
    const identity_memory_resource *o =
        dynamic_cast<const identity_memory_resource *>(&other);

    return o != nullptr && m_upstream.is_equal(o->m_upstream);
}
}  // namespace vecmem
