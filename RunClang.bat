:: Okay
::clang Src/* Drivers/Windows/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v


:: Linker error
::clang Src/* Drivers/Windows/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v -o Out/ClangCompiled.exe


:: Half okay, there is no report file
::clang --analyze --analyzer-output html Src/* Drivers/Windows/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v 


clang --analyze --analyzer-output html Src/* Src/Common/Helper/* Drivers/Windows/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v
::  -o Out - only in one file?

:: TODO: Add other source directories!

:: -v ~verbose
:: -S Only preprocess+compile
:: ?? -o Out/ClangCompiled.exe

:: Check "--analyze" argument
::clang Src/main.c -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 --analyze -o Out/ClangAnalysis.log


:: Check "clang-check"
::clang-check -analyze


:: cpp: clang++ -std=c++14
