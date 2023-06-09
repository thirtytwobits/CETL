#
# Copyright (C) OpenCyphal Development Team  <opencyphal.org>
# Copyright Amazon.com Inc. or its affiliates.
# SPDX-License-Identifier: MIT
#

# Toolchain for using gcc to cross-compile for ARM
#
set(LOCAL_TOOLCHAIN         arm-none-eabi)

set(CMAKE_C_COMPILER        ${LOCAL_TOOLCHAIN}-gcc      CACHE INTERNAL "C Compiler")
set(CMAKE_LINKER            ${LOCAL_TOOLCHAIN}-ld       CACHE INTERNAL "Linker")
set(CMAKE_CXX_COMPILER      ${LOCAL_TOOLCHAIN}-g++      CACHE INTERNAL "C++ Compiler")
set(CMAKE_AR                ${LOCAL_TOOLCHAIN}-ar       CACHE INTERNAL "Archiver")
set(CMAKE_ASM_COMPILER      ${LOCAL_TOOLCHAIN}-as       CACHE INTERNAL "Assembler")
set(CMAKE_OBJCOPY           ${LOCAL_TOOLCHAIN}-objcopy  CACHE INTERNAL "Object Copy")
set(CMAKE_RANLIB            ${LOCAL_TOOLCHAIN}-ranlib   CACHE INTERNAL "Library Indexer")
set(CMAKE_STRIP             ${LOCAL_TOOLCHAIN}-strip    CACHE INTERNAL "Stripper")

# TODO:
#set(CMAKE_FIND_ROOT_PATH  /path/to/cmsis/perhaps)

# keep find from finding things (in the host environment)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

# don't try to build executables when checking the compilers.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
