/** VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Local include(s).
#include "vecmem/containers/data/jagged_vector_view.hpp"
#include "vecmem/containers/data/vector_view.hpp"

// System include(s).
#include <cstddef>

/// Perform a linear transformation using the received vectors
void linearTransform(vecmem::data::vector_view<const int> constants,
                     vecmem::data::vector_view<const int> input,
                     vecmem::data::vector_view<int> output);

/// Function incrementing the elements of the received vector using atomics
void atomicTransform(unsigned int iterations,
                     vecmem::data::vector_view<int> vec);

/// Function filtering elements of an input vector into an output vector
void filterTransform(vecmem::data::vector_view<const int> input,
                     vecmem::data::vector_view<int> output);

/// Function filtering elements of an input vector into an output vector
void filterTransform(vecmem::data::jagged_vector_view<const int> input,
                     unsigned int max_vec_size,
                     vecmem::data::jagged_vector_view<int> output);
