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
  git clone https://github.com/Fasten90/SourceCodeChecker.git
)


:: Go into the root
cd ..


:: Run SCC
:: Default config: scc_config.json
echo Run SourceCodeChecker
echo ---------------------
python Tools/SourceCodeChecker/SourceCodeChecker.py --project-path=..\\..\\AtollicWorkspace\\FastenHomeAut --source-file-path='Src\**,Inc\**,Drivers\x86\**'

