if ($IsWindows) {
    cmake . `
      -DCMAKE_PREFIX_PATH=C:/Qt/6.8.2/msvc2022_64 `
      -DCMAKE_CXX_STANDARD=17 `
      -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" `
      -B build
    cmake --build build --config Release --parallel 32
}

if ($IsLinux -or $IsMacOS) {
    cmake -B build
    cmake --build build --config Release --parallel 32
}

if ($IsWindows) {
    C:\Qt\6.8.2\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe

    (Get-FileHash build/Release/SQLiteQueryAnalyzer.exe -Algorithm SHA1 | Select-Object).Hash 
    | Set-Content build/Release/SQLiteQueryAnalyzer.exe.sha1

    (Get-FileHash build/Release/SQLiteQueryAnalyzer.exe -Algorithm MD5 | Select-Object).Hash 
    | Set-Content build/Release/SQLiteQueryAnalyzer.exe.md5

    ../../deps/innosetup/ISCC.exe setup.iss
}

if ($IsMacOS) {
    macdeployqt build/SQLiteQueryAnalyzer.app -dmg
}
