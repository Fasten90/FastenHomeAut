echo Run Fasten SCC

# Go to Tools
mkdir -p Tools
cd Tools

# Clone SCC
# PIPELINE_WORKSPACE - Define available at Pipeline (e.g. Azure)
git clone https://github.com/Fasten90/SourceCodeChecker.git

# Go into the root
cd ..

# Run SCC
# Default config: scc_config.json
echo Run SourceCodeChecker
echo ---------------------
python Tools/SourceCodeChecker/SourceCodeChecker.py --source-file-path="Src\**,Inc\**,Drivers\x86\**"

