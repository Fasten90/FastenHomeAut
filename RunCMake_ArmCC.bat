@echo off
echo Run CMake - ArmCC - Keil uVision


:: Go to build directory
mkdir Out
cd Out
mkdir CMakeBuild_ArmCC
cd CMakeBuild_ArmCC


:: Run CMake
cmake ^
-D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/Keil_v5/ARM/ARMCC/bin/armcc.exe" ^
-D"CMAKE_FASTENHOMEAUT_TARGET=STM32F0_arm-armcc" ^
CC=armcc -G "MinGW Makefiles" ../..

:: Run make
make VERBOSE=1


:: Test
set filename="FastenHomeAut.exe"


set test_executing_result_wrong=0


:: Check executable file is exists?
if exist %filename%  (
    echo Executable result exists
) else (
    echo Executable result does not exist
    set test_executing_result_wrong=1
    goto:error_exit
)


:: Go to original directory
cd ../..


if %test_executing_result_wrong% NEQ 0 (
    echo There was error!
    echo Exit value:
    echo %test_executing_result_wrong%
) else (
    echo Script has run successfully
)

::exit %test_executing_result_wrong%
