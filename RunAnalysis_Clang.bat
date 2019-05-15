:: Analyze - okay
clang --analyze --analyzer-output html Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/x86/Src/* -IDrivers/x86/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v

