if ($IsWindows) {
    cmake . -DCMAKE_PREFIX_PATH=C:/Qt/6.8.2/msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" -B build
    cmake --build build --config Release
    C:\Qt\6.8.2\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe
    ../../deps/innosetup/ISCC.exe setup.iss
} 

if ($IsLinux -or $IsMacOS) {
    cmake -B build
    cmake --build build --config Release
}

if ($IsMacOS) {
    macdeployqt build/SQLiteQueryAnalyzer.app -dmg
}