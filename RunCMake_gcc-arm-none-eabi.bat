echo Run CMake - gcc-arm-none-eabi


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildGccArmNoneEabi
cd CMakeBuildGccArmNoneEabi


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/arm-atollic-eabi-gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/arm-atollic-eabi-g++.exe" ^
-G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH="../../toolchain.cmake" ../..

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
