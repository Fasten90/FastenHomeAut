@echo off
echo Run CMake - MinGW - GCC


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuild_MinGW_gcc
cd CMakeBuild_MinGW_gcc


:: Run CMake
::cmake CC=gcc -G "MinGW Makefiles" ../..
:: Shall be added the '-DCMAKE_SH="CMAKE_SH-NOTFOUND"' because in Azure pipeline there was added the git, and we shall skip that
cmake CC=gcc -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" ../..


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

:: Clone SCC
if defined PIPELINE_WORKSPACE (
  echo PIPELINE_WORKSPACE is defined 
  exit %test_executing_result_wrong%
)
