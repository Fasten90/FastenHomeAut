echo Run CMake - gcc-arm-atollic-eabi


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildGccArmAtollicEabi
cd CMakeBuildGccArmAtollicEabi


:: Add path
::setx path "%path%;c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/"

:: Old time


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-G "Unix Makefiles" ^
-D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/arm-atollic-eabi-gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/arm-atollic-eabi-g++.exe" ^
-DCMAKE_TOOLCHAIN_FILE:PATH="../../Toolchain_ST32F0_arm-atollic-eabi-gcc.cmake" ^
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
