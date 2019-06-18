@echo off
echo Run Fasten SCC


:: Go to Out/CMakeBuild_Tcc
mkdir Tools
cd Out
mkdir SourceCodeChecker
cd SourceCodeChecker


:: Clone SCC
git clone https://fasten90@bitbucket.org/fasten90/sourcecodechecker.git


cd ../..


:: Run SCC
:: Default config: scc_config.json
python Tools/SourceCodeChecker/SourceCodeChecker.py

