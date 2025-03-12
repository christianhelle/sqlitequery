#Remove-Item .\release -Recurse -Force
#qmake SQLiteAnalyzer.pro -config release
cmake . -DCMAKE_PREFIX_PATH=C:/Qt/6.8.2/msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" -B build
cmake --build build --config Release
# mingw32-make -j32
# mkdir .\release\bin
# Move-Item .\release\SQLiteQueryAnalyzer.exe .\release\bin
C:\Qt\6.8.2\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe