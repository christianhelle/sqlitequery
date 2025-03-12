Remove-Item .\release -Recurse -Force
qmake SQLiteAnalyzer.pro -config release
mingw32-make -j32
mkdir .\release\bin
Move-Item .\release\SQLiteQueryAnalyzer.exe .\release\bin
windeployqt .\release\bin\SQLiteQueryAnalyzer.exe