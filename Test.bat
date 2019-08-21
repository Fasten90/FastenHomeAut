echo Test:

cd

dir

SET

echo Test echo redirection + Artifact

echo This test will not showed in console, but in file > logfile.txt 2>&1

dir >> logfile.txt 2>&1

