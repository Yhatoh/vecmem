# VecMem project, part of the ACTS project (R&D line)
#
# (c) 2021 CERN for the benefit of the ACTS project
#
# Mozilla Public License Version 2.0

# Enable CUDA as a language.
enable_language( CUDA )

# Project include(s).
include( vecmem-compiler-options-cpp )
include( vecmem-compiler-options-cuda )

# External dependency/dependencies.
find_package( CUDAToolkit REQUIRED )

# Test all of the CUDA library's features.
vecmem_add_test( cuda
   "test_cuda_memory_resources.cpp"
   "test_cuda_containers.cpp" "test_cuda_containers_kernels.cuh"
   "test_cuda_containers_kernels.cu"
   "test_cuda_jagged_vector_view.cpp"
   "test_cuda_jagged_vector_view_kernels.cu"
   "test_cuda_jagged_vector_view_kernels.cuh"
   "${CMAKE_CURRENT_SOURCE_DIR}/../../cuda/src/utils/cuda_error_handling.hpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/../../cuda/src/utils/cuda_error_handling.cpp"
   LINK_LIBRARIES CUDA::cudart vecmem::core vecmem::cuda GTest::gtest_main
                  vecmem_testing_common )
