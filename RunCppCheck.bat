echo Run Cppcheck

cppcheck Src Inc Drivers -iDrivers/CMSIS -iDrivers/STM32F0xx_HAL_Driver -iDrivers/STM32F4xx_HAL_Driver --enable=all --inconclusive --suppressions-list=cppcheck_suppressions.txt --template="[{file}:{line}]:\t({severity})\t{message}" --inline-suppr --std=c11 2> Out/CppcheckErrors.txt

start notepad++ "Out/CppcheckErrors.txt"
