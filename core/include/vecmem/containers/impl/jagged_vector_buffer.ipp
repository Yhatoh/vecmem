/*
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// System include(s).
#include <cassert>
#include <cstddef>
#include <numeric>
#include <vector>

namespace {

/// Helper conversion function
template <typename TYPE>
std::vector<std::size_t> get_sizes(
    const vecmem::data::jagged_vector_view<TYPE>& jvv) {

    std::vector<std::size_t> result(jvv.m_size);
    for (std::size_t i = 0; i < jvv.m_size; ++i) {
        result[i] = jvv.m_ptr[i].size();
    }
    return result;
}

/// Function allocating memory for @c vecmem::data::jagged_vector_buffer
template <typename TYPE>
std::unique_ptr<typename vecmem::data::jagged_vector_buffer<TYPE>::value_type,
                vecmem::details::deallocator>
allocate_jagged_buffer_outer_memory(
    typename vecmem::data::jagged_vector_buffer<TYPE>::size_type size,
    vecmem::memory_resource& resource) {

    const std::size_t byteSize =
        size *
        sizeof(typename vecmem::data::jagged_vector_buffer<TYPE>::value_type);
    return {
        size == 0
            ? nullptr
            : static_cast<
                  typename vecmem::data::jagged_vector_buffer<TYPE>::pointer>(
                  resource.allocate(byteSize)),
        {byteSize, resource}};
}

/// Function allocating memory for @c vecmem::data::jagged_vector_buffer
template <typename TYPE>
std::unique_ptr<char, vecmem::details::deallocator>
allocate_jagged_buffer_inner_memory(const std::vector<std::size_t>& sizes,
                                    vecmem::memory_resource& resource,
                                    bool isResizable) {

    typename vecmem::data::jagged_vector_buffer<TYPE>::size_type byteSize =
        std::accumulate(sizes.begin(), sizes.end(),
                        static_cast<std::size_t>(0)) *
        sizeof(TYPE);
    if (isResizable) {
        byteSize +=
            sizes.size() * sizeof(typename vecmem::data::jagged_vector_buffer<
                                  TYPE>::value_type::size_type);
    }
    return {byteSize == 0 ? nullptr
                          : static_cast<char*>(resource.allocate(byteSize)),
            {byteSize, resource}};
}

}  // namespace

namespace vecmem {
namespace data {

template <typename TYPE>
template <typename OTHERTYPE,
          std::enable_if_t<std::is_convertible<TYPE, OTHERTYPE>::value, bool> >
jagged_vector_buffer<TYPE>::jagged_vector_buffer(
    const jagged_vector_view<OTHERTYPE>& other, memory_resource& resource,
    memory_resource* host_access_resource)
    : jagged_vector_buffer(::get_sizes(other), resource, host_access_resource) {

}

template <typename TYPE>
jagged_vector_buffer<TYPE>::jagged_vector_buffer(
    const std::vector<std::size_t>& sizes, memory_resource& resource,
    memory_resource* host_access_resource)
    : base_type(sizes.size(), nullptr),
      m_outer_memory(::allocate_jagged_buffer_outer_memory<TYPE>(
          (host_access_resource == nullptr ? 0 : sizes.size()), resource)),
      m_outer_host_memory(::allocate_jagged_buffer_outer_memory<TYPE>(
          sizes.size(),
          (host_access_resource == nullptr ? resource
                                           : *host_access_resource))),
      m_inner_memory(
          ::allocate_jagged_buffer_inner_memory<TYPE>(sizes, resource, false)) {

    // Point the base class at the newly allocated memory.
    base_type::m_ptr =
        ((host_access_resource != nullptr) ? m_outer_memory.get()
                                           : m_outer_host_memory.get());

    // Set up the host accessible memory array.
    std::ptrdiff_t ptrdiff = 0;
    for (std::size_t i = 0; i < sizes.size(); ++i) {
        new (host_ptr() + i)
            value_type(static_cast<typename value_type::size_type>(sizes[i]),
                       reinterpret_cast<TYPE*>(m_inner_memory.get() + ptrdiff));
        ptrdiff += sizes[i] * sizeof(TYPE);
    }
}

template <typename TYPE>
jagged_vector_buffer<TYPE>::jagged_vector_buffer(
    const std::vector<std::size_t>& sizes,
    const std::vector<std::size_t>& capacities, memory_resource& resource,
    memory_resource* host_access_resource)
    : base_type(sizes.size(), nullptr),
      m_outer_memory(::allocate_jagged_buffer_outer_memory<TYPE>(
          (host_access_resource == nullptr ? 0 : sizes.size()), resource)),
      m_outer_host_memory(::allocate_jagged_buffer_outer_memory<TYPE>(
          sizes.size(),
          (host_access_resource == nullptr ? resource
                                           : *host_access_resource))),
      m_inner_memory(::allocate_jagged_buffer_inner_memory<TYPE>(
          capacities, resource, true)) {

    // Some sanity check.
    assert(sizes.size() == capacities.size());

    // Point the base class at the newly allocated memory.
    base_type::m_ptr =
        ((host_access_resource != nullptr) ? m_outer_memory.get()
                                           : m_outer_host_memory.get());

    // Set up the host accessible memory array.
    std::ptrdiff_t ptrdiff =
        (capacities.size() * sizeof(typename value_type::size_type));
    for (std::size_t i = 0; i < capacities.size(); ++i) {
        new (host_ptr() + i) value_type(
            static_cast<typename value_type::size_type>(capacities[i]),
            reinterpret_cast<typename value_type::size_pointer>(
                m_inner_memory.get() +
                i * sizeof(typename value_type::size_type)),
            reinterpret_cast<TYPE*>(m_inner_memory.get() + ptrdiff));
        ptrdiff += capacities[i] * sizeof(TYPE);
    }
}

template <typename TYPE>
auto jagged_vector_buffer<TYPE>::host_ptr() const -> pointer {

    return m_outer_host_memory.get();
}

}  // namespace data

template <typename TYPE>
data::jagged_vector_view<TYPE>& get_data(
    data::jagged_vector_buffer<TYPE>& data) {

    return data;
}

template <typename TYPE>
const data::jagged_vector_view<TYPE>& get_data(
    const data::jagged_vector_buffer<TYPE>& data) {

    return data;
}

}  // namespace vecmem
