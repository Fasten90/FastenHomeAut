@echo off
echo Run CMake - Clang LLVM


:: Go to build directory
mkdir Out
cd Out

rmdir /S /Q CMakeBuild_Clang

mkdir CMakeBuild_Clang
cd CMakeBuild_Clang


:: Run CMake
:: Example:
::cmake CC=gcc -G "MinGW Makefiles" ../..

::set CC=/usr/bin/clang
::set CXX=/usr/bin/clang++

set CC=clang
set CXX=clang++
cmake CC=clang CXX=clang++ -D"CMAKE_C_COMPILER:FILEPATH=clang" -D"CMAKE_CXX_COMPILER:FILEPATH=clang++" ../..

:: Test clang-cl
::cmake CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..

:: Test clang-cl
::  "C:/Programs/Engineer/LLVM/bin/clang-cl.exe"
:: is not able to compile a simple test program.
::set CC=clang-cl
::set CXX=clang-cl
::cmake  -G "MinGW Makefiles" -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
::cmake -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..


:: Run make
:: make shall be in the PATH
make VERBOSE=1


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


:: Start compiled exe
call %filename%


if %ERRORLEVEL% EQU 0 (
    echo Unittest executing was successful
) else (
    echo Unittest failed!
    set test_executing_result_wrong=2
)


:error_exit

:: Go to original directory
cd ..
cd ..

if %test_executing_result_wrong% NEQ 0 (
    echo There was error!
    echo Exit value:
    echo %test_executing_result_wrong%
) else (
    echo Script has run successfully
)

::exit %test_executing_result_wrong%
