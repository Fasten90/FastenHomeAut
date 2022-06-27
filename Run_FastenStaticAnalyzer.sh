#!/bin/bash

includes="-IInc/Common -IInc/Common/Handler -IInc/Common/Helper -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/SelfTest"
args="-IDrivers/x86/Inc -DCONFIG_USE_PANEL_PC=1"
# Works, but generalize:
#python FastenStaticAnalyzer.py --source="../Src/Common/Handler/CommandHandler.c" --prepocessor="gcc" --preprocessor_args="-std=c99 -nostdinc -E -D_Atomic(_arg)=_arg -Ipycparser/utils/fake_libc_include ${args} ${includes}"
default_analyzer_args="-std=c99 -nostdinc -E -D_Atomic(_arg)=_arg -IFastenStaticAnalyzer/pycparser/utils/fake_libc_include"

#git ls-files -z | grep -e ".*\.c" | xargs python FastenStaticAnalyzer/FastenStaticAnalyzer.py --source={} --prepocessor="gcc" --preprocessor_args="${default_analyzer_args} ${args} ${includes}"
# TODO: Not the best pattern for c files
for file in $(git ls-files -x *c) ; 
do
    python FastenStaticAnalyzer/FastenStaticAnalyzer.py --source=$file --preprocessor="gcc" --preprocessor_args="${default_analyzer_args} ${args} ${includes}"
done
