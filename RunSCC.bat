@echo off
echo Run Fasten SCC


:: Go to Tools
mkdir Tools
cd Tools
::mkdir SourceCodeChecker
::cd SourceCodeChecker


:: Clone SCC
:: PIPELINE_WORKSPACE - Define available at Pipeline (e.g. Azure)
if defined PIPELINE_WORKSPACE (
  echo PIPELINE_WORKSPACE is defined, download SourceCodeChecker
  git clone https://fasten90@bitbucket.org/fasten90/sourcecodechecker.git
)


:: Go into the root
cd ..


:: Run SCC
:: Default config: scc_config.json
echo Run SourceCodeChecker
echo ---------------------
python Tools/SourceCodeChecker/SourceCodeChecker.py

