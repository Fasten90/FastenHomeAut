#!/bin/bash
set -e


echo Run CMake - gcc-arm-none-eabi


# Go to Out/CMakeBuild
mkdir -p Out/CMakeBuildGccArmNoneEabi && cd Out/CMakeBuildGccArmNoneEabi

# TODO: Add precheck: arm-none-eabi-gcc

# Run CMake
cmake \
-D"CMAKE_MAKE_PROGRAM:FILEPATH=make" \
-G "Unix Makefiles" \
-D"CMAKE_C_COMPILER:FILEPATH=arm-none-eabi-gcc" \
-D"CMAKE_CXX_COMPILER:FILEPATH=arm-none-eabi-g++" \
-DCMAKE_TOOLCHAIN_FILE:PATH="../../Toolchain_ST32F0_arm-none-eabi-gcc.cmake" \
../..

# cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH="../../toolchain.cmake" ../..


# Run make
# make
make VERBOSE=1


# TODO: Check binary is exists


# Go to original directory
cd ../..
