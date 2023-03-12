#
# Copyright (C) OpenCyphal Development Team  <opencyphal.org>
# Copyright Amazon.com Inc. or its affiliates.
# SPDX-License-Identifier: MIT
#

# Toolchain for using clang on what-ever-platform-this-is (aka "native").
# CMake and/or Ubuntu defaults to gcc.
#
set(CMAKE_C_COMPILER clang CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER clang++ CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER clang CACHE FILEPATH "assembler")
set(CMAKE_CXX_STANDARD ${CETLVAST_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Signal to unit tests that the coverage data will be generated by
# clang so they should try to use llvm-cov if it's available.
# Things are weird between gcov and llvm-cov right now but this
# seems to work in our Ubuntu 18 Docker container, so...?
set(CETLVAST_USE_LLVM_COV ON CACHE BOOL "Enable gcov compatibility with lcov coverage tools.")