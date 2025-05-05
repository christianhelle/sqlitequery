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

if ($IsLinux) {
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/sqlitequery
    cmake --build build --config Release --parallel 32
    cmake --install build
    if ($package) {
        tar -czf SQLiteQueryAnalyzer.tar.gz /tmp/sqlitequery .
        cpack -G 7Z
        cpack -G ZIP
        cpack -G TBZ2
        cpack -G TGZ
        cpack -G TXZ
        cpack -G TZ
        cpack -G DEB
        cpack -G RPM
        cpack -G IFW
    }
}

if ($IsMacOS -And $package) {
    
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/sqlitequery
    cmake --build build --config Release --parallel 32
    if ($package) {
        macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
    }
}
