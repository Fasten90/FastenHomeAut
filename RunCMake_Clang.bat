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

::set CC=clang
::set CXX=clang++
::cmake CC=clang CXX=clang++ -D"CMAKE_C_COMPILER:FILEPATH=clang" -D"CMAKE_CXX_COMPILER:FILEPATH=clang++" ../..

::-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" 
:: -G "Ninja"  - tested, not compile  - not set

::cmake -D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
:: this use default NMake

:: NMake
::cmake -G "NMake Makefiles" -D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
:: wrong - Make not run correct

:: Unix Makefiles
::cmake -G "Unix Makefiles" -D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
:: Wrong - link fail


::cmake -G "Unix Makefiles" -D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" -D"CMAKE_C_COMPILER:FILEPATH=clang" -D"CMAKE_CXX_COMPILER:FILEPATH=clang++" ../..
:: Suggest clang-cl

:: MSVC

:: https://developercommunity.visualstudio.com/content/problem/495609/cmake-integration-diary-vs2017-and-vs2019-preview.html
:: mkdir vs2019-build-bundled-cmake  cd vs2019-build-bundled-cmake  "C:\Program Files (x86)\Microsoft Visual Studio\2019\Preview\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -G "Visual Studio 15 2017 Win64" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" ..  


::cmake -G "Visual Studio 16 2019" -A Win32 -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=C:\Programs\Engineer\LLVM-8.0.0-win32\bin\clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=C:\Programs\Engineer\LLVM-8.0.0-win32\bin\clang-cl" ../..
:: MSVC used..

::cmake -G "Visual Studio 16 2019" -A Win32 -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -T LLVM CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=C:\Programs\Engineer\LLVM-8.0.0-win32\bin\clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=C:\Programs\Engineer\LLVM-8.0.0-win32\bin\clang-cl" ../..
:: -T LLVM  --> LLVM toolset not found


::cmake -G "Visual Studio 16 2019" -A Win32 -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" ../..
:: Cannot build


::cmake -G "Visual Studio 16 2019" -A Win64 -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" ../..
:: C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\Microsoft.Common.CurrentVersion.targets


cmake -G "Visual Studio 16 2019" -A Win32 -T LLVM CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=C:\Programs\Engineer\LLVM-8.0.0-win64\bin\clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=C:\Programs\Engineer\LLVM-8.0.0-win64\bin\clang-cl" ../..
:: Die on not supported toolset by VS2019
:: C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Microsoft\VC\v160\Microsoft.CppBuild.targets(379,5): error MSB8020: The build tools for LLVM (Platform Toolset = 'LLVM') cannot be found


::cmake -G "Visual Studio 16 2019" -A Win32 -T LLVM ../..
:: Die on not supported toolset by VS2019
:: C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Microsoft\VC\v160\Microsoft.CppBuild.targets(379,5): error MSB8020: The build tools for LLVM (Platform Toolset = 'LLVM') cannot be found

:: cmake -G "Visual Studio 16 2019" -A Win32 -T LLVM -t x86_64-pc-windows-msvc ../..
::cmake -G "Visual Studio 16 2019" -A Win32 -T LLVM-vs2019 ../..
:: cmake .. -TLLVM-vs2014 -G"Visual Studio 14 2015"
::cmake -G "Visual Studio 16 2019" -A Win32 -T llvm CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
::cmake -G "Visual Studio 16 2019" -A Win32 CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
::cmake  -D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" -G "Visual Studio 16 2019" -A Win32 CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
:: Want to use MSVC
::cmake CC=clang-cl CXX=clang-cl -D"CMAKE_C_COMPILER:FILEPATH=clang-cl" -D"CMAKE_CXX_COMPILER:FILEPATH=clang-cl" ../..
:: Visual Studio 15 2017 Win64
::cmake -G "Visual Studio 16 2019" -A Win32
::cmake -G "Visual Studio 16 2019" -A x64

::-- Check for working C compiler: C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/MSVC/14.20.27508/bin/Hostx64/x64/cl.exe
::-- Check for working CXX compiler: C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/MSVC/14.20.27508/bin/Hostx64/x64/cl.exe


::cmake -G"Visual Studio 14 2015" -T"LLVM-vs2014" ..



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
