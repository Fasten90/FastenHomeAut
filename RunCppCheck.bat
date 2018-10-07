echo Run Cppcheck

:: cppcheck shall be added to the PATH

cppcheck Src Inc Drivers ^
-IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest ^
-IDrivers/STM32F0xx_HAL_Driver/Inc -IDrivers/STM32F0xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F0xx/Include -IDrivers/CMSIS/Include ^
-iDrivers/CMSIS -iDrivers/STM32F0xx_HAL_Driver -iDrivers/STM32F4xx_HAL_Driver ^
--enable=all --inconclusive --suppressions-list=cppcheck_suppressions.txt --template="[{file}:{line}]:\t({severity})\t{message}" --inline-suppr --std=c99 --force --check-config 2> Out/CppcheckErrorsC99.txt

cppcheck Src Inc Drivers ^
-IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Game -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest ^
-IDrivers/STM32F0xx_HAL_Driver/Inc -IDrivers/STM32F0xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F0xx/Include -IDrivers/CMSIS/Include ^
-iDrivers/CMSIS -iDrivers/STM32F0xx_HAL_Driver -iDrivers/STM32F4xx_HAL_Driver ^
--enable=all --inconclusive --suppressions-list=cppcheck_suppressions.txt --template="[{file}:{line}]:\t({severity})\t{message}" --inline-suppr --std=c11 --force --check-config 2> Out/CppcheckErrorsC11.txt

:: Open Error files with Notepad++
start notepad++ "Out/CppcheckErrorsC99.txt"
start notepad++ "Out/CppcheckErrorsC11.txt"
