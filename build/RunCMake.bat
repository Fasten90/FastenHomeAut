::cmake -D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" -D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/mingw64/bin/gcc.exe" -D"CMAKE_CXX_COMPILER:FILEPATH=c:/Programs/Engineer/mingw64/bin/g++.exe" CC=gcc -G "MinGW Makefiles" ..


cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/mingw64/bin/gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=c:/Programs/Engineer/mingw64/bin/g++.exe" ^
CC=gcc -G "MinGW Makefiles" ..


:: CMAKE_C_FLAGS:"-IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include" ^



:: FILEPATH

:: cmake -D"CMAKE_MAKE_PROGRAM:PATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" -D"CMAKE_C_COMPILER:PATH=c:/Programs/Engineer/mingw64/bin/gcc.exe" -D"CMAKE_CXX_COMPILER:PATH=c:/Programs/Engineer/mingw64/bin/g++.exe" CC=gcc -G "MinGW Makefiles" ..

:: c:\Programs\Engineer\mingw64\bin\gcc.exe
:: c:\Programs\Engineer\TrueSTUDIO for STM32 9.0.1\PCTools\bin\gcc.exe

:: c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/Tools/make.exe


:: Run make
:: c:\Programs\Engineer\Utils\CompilerTools\make.exe
c:\Programs\Engineer\Utils\CompilerTools\make.exe VERBOSE=1
