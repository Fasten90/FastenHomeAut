C:\Programs\Engineer\tcc\tcc.exe ^
-DCONFIG_USE_PANEL_PC=1 -IInc -IInc/Common -IDrivers/Windows/Inc/ -IDrivers/Windows/Legacy -IInc/Common/Handler -IInc/Common/Helper -IInc/Communication -IInc/Modules -IInc/List -IInc/Game -IInc/Display -IInc/SelfTest -IInc/HwTester ^
-c Src/Common/Handler/TaskHandler.c ^
-o Out/TccBuild/taskhandler.o