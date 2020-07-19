echo Run CMake - gcc-arm-none-eabi


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildGccArmNoneEabi
cd CMakeBuildGccArmNoneEabi



:: TODO: Add precheck: arm-none-eabi-gcc.exe


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=make.exe" ^
-G "Unix Makefiles" ^
-D"CMAKE_C_COMPILER:FILEPATH=arm-none-eabi-gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=arm-none-eabi-g++.exe" ^
-DCMAKE_TOOLCHAIN_FILE:PATH="../../Toolchain_ST32F4_arm-none-eabi-gcc.cmake" ^
../..

:: cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH="../../toolchain.cmake" ../..


:: Run make
:: make
make VERBOSE=1


:: TODO: Check binary is exists


:: Go to original directory
cd ..
cd ..
