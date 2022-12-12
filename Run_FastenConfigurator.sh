#!/bin/bash
set -e

# FastenConfigurator

FASTENCCONFIGURATOR_PATH="FastenCConfigurator"
mkdir -p Tools
pushd Tools
if [[ -d "$FASTENCCONFIGURATOR_PATH" ]]; then
    pushd $FASTENCCONFIGURATOR_PATH
    git pull
    popd
else
    #git clone git@github.com:Fasten90/FastenCConfigurator.git
    git clone https://github.com/fasten90/FastenCConfigurator
fi
popd

python "Tools/${FASTENCCONFIGURATOR_PATH}/FastenCConfigurator.py"

# Iterate on configs
export FASTENCCONFIGURATOR=true
for config_item in config_*
do
    echo "Next config: ${config_item}"
    export FASTENCCONFIGURATOR_CONFIG=$config_item
    ./RunCMake_gcc-arm-none-eabi.bat
done
