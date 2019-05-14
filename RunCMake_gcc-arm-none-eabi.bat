echo Run CMake - gcc-arm-none-eabi


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildGccArmNoneEabi
cd CMakeBuildGccArmNoneEabi




:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-G "Unix Makefiles" ^
-D"CMAKE_C_COMPILER:FILEPATH=C:/Programs/Engineer/GNU_Tools_ARM_Embedded_8_2018-q4-major/bin/arm-none-eabi-gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=C:/Programs/Engineer/GNU_Tools_ARM_Embedded_8_2018-q4-major/bin/arm-none-eabi-g++.exe" ^
-DCMAKE_TOOLCHAIN_FILE:PATH="../../Toolchain_ST32F0_arm-none-eabi-gcc.cmake" ^
../..

:: cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH="../../toolchain.cmake" ../..


:: Run make
:: c:\Programs\Engineer\Utils\CompilerTools\make.exe
c:\Programs\Engineer\Utils\CompilerTools\make.exe VERBOSE=1


:: TODO: Delete
:: Start compiled exe
::FastenHomeAut.exe


:: Go to original directory
cd ..
cd ..
