:: Test MinGW32
c:\Programs\Engineer\mingw32\bin\gcc.exe ^
-DCONFIG_USE_PANEL_PC=1 -IInc -IInc/Common -IDrivers/Windows/Inc/ -IDrivers/Windows/Legacy -IInc/Common/Handler -IInc/Common/Helper -IInc/Communication -IInc/Modules -IInc/List -IInc/Game -IInc/Display -IInc/SelfTest -IInc/HwTester ^
-o Out\Test\MinGW32test.o ^
-c Src/Common/Handler/TaskHandler.c


:: Test MinGW64
c:\Programs\Engineer\mingw64\bin\gcc.exe ^
-DCONFIG_USE_PANEL_PC=1 -IInc -IInc/Common -IDrivers/Windows/Inc/ -IDrivers/Windows/Legacy -IInc/Common/Handler -IInc/Common/Helper -IInc/Communication -IInc/Modules -IInc/List -IInc/Game -IInc/Display -IInc/SelfTest -IInc/HwTester ^
-o Out\Test\MinGW64test.o ^
-c Src/Common/Handler/TaskHandler.c
