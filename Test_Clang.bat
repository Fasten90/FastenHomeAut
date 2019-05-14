:: Compile - per file - okay
::clang Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/x86/Src/* -IDrivers/x86/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v


:: Compile - out.exe - Linker error
::clang Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/x86/Src/* -IDrivers/x86/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -v -o Out/ClangCompiled.exe


:: Analyze - okay
clang --analyze --analyzer-output html Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/x86/Src/* -IDrivers/x86/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v


:: Analyze - error with "only one file"
::clang --analyze --analyzer-output html Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Game/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/x86/Src/* -IDrivers/x86/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v -o Out
:: -o Out - only in one file?


:: Clang parameters
:: -v ~verbose
:: -S Only preprocess+compile
:: ?? -o Out/ClangCompiled.exe


:: TODO:
:: c: C99
:: cpp: clang++ -std=c++14
