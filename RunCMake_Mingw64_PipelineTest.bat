echo Run CMake - MinGW64


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildMinGW64
cd CMakeBuildMinGW64


:: Run CMake
::cmake CC=gcc ..
cmake CC=gcc -G "MinGW Makefiles" ../..


:: Run make
make VERBOSE=1


:: TODO: Not Start if there is error


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
