@echo off
echo Run Fasten SCC


:: Go to Out/CMakeBuild_Tcc
mkdir Tools
cd Tools
::mkdir SourceCodeChecker
::cd SourceCodeChecker


:: Clone SCC
if defined PIPELINE_WORKSPACE (
  echo PIPELINE_WORKSPACE is defined, download SourceCodeChecker
  git clone https://fasten90@bitbucket.org/fasten90/sourcecodechecker.git
)


cd
dir

cd ..

cd
dir


:: Run SCC
:: Default config: scc_config.json
echo Run SourceCodeChecker
echo ---------------------
python Tools/SourceCodeChecker/SourceCodeChecker.py

