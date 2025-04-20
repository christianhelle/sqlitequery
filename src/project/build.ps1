param (
    [switch]
    $package = $false
)

if ($IsWindows) {
    cmake . -DCMAKE_PREFIX_PATH=C:/Qt/6.9.0/msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" -B build
    cmake --build build --config Release --parallel 32
    if ($package) {
        C:\Qt\6.9.0\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe
        ../../deps/innosetup/ISCC.exe setup.iss
    }
} 

if ($IsLinux -or $IsMacOS) {
    cmake -B build
    cmake --build build --config Release --parallel 32
}

if ($IsMacOS -And $package) {
    macdeployqt build/SQLiteQueryAnalyzer.app -dmg
}
