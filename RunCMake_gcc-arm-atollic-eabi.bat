echo Run CMake - gcc-arm-atollic-eabi


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildGccArmAtollicEabi
cd CMakeBuildGccArmAtollicEabi


:: Add path
:: TODO: Delete
::setx path "%path%;c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/"


:: TODO: Add precheck
:: arm-atollic-eabi-gcc.exe shall be on PATH:
:: E.g. c:\Programs\Engineer\TrueSTUDIO for STM32 9.3.0\ARMTools\bin\arm-atollic-eabi-gcc.exe


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=make.exe" ^
-G "Unix Makefiles" ^
-D"CMAKE_C_COMPILER:FILEPATH=arm-atollic-eabi-gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=arm-atollic-eabi-g++.exe" ^
-DCMAKE_TOOLCHAIN_FILE:PATH="../../Toolchain_ST32F0_arm-atollic-eabi-gcc.cmake" ^
../..

:: cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH="../../toolchain.cmake" ../..


:: Run make
make VERBOSE=1


:: TODO: Check binary is exists


:: Go to original directory
cd ..
cd ..
