#!/bin/bash
set -e


INCLUDES="-IInc/Common -IInc/Common/Handler -IInc/Common/Helper -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/SelfTest"
ARGS="-IDrivers/x86/Inc -DCONFIG_USE_PANEL_PC=1"
DEFAULT_ANALYZER_ARGS="-std=c99 -nostdinc -E -D_Atomic(_arg)=_arg -DINLINE_FUNCTION= -IFastenStaticAnalyzer/pycparser/utils/fake_libc_include"
# pycparser fail on "#define INLINE_FUNCTION     inline __attribute__((always_inline))"


#git ls-files -z | grep -e ".*\.c" | xargs python FastenStaticAnalyzer/FastenStaticAnalyzer.py --source={} --prepocessor="gcc" --preprocessor_args="${default_analyzer_args} ${args} ${includes}"
# TODO: Not the best pattern for c files
for file in $(git ls-files -x *c) ; 
do
    python -u FastenStaticAnalyzer/FastenStaticAnalyzer.py --source=$file --preprocessor="gcc" --preprocessor_args="${DEFAULT_ANALYZER_ARGS} ${ARGS} ${INCLUDES}" --delete_temporary_files --export_file="StaticAnalysisResult.csv" --extend_export_file
done
