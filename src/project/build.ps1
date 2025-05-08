param (
    [switch]
    $Package = $false,

    [switch]
    $Install = $false
)

if ($IsWindows) {
    cmake . -DCMAKE_PREFIX_PATH=C:/Qt/6.9.0/msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" -B build
    cmake --build build --config Release --parallel 32
    if ($Package) {
        C:\Qt\6.9.0\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe
        ../../deps/innosetup/ISCC.exe setup.iss
    }
} 

if ($IsLinux) {
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./linux/
    cmake --build build --config Release --parallel 32
    cmake --install build
    cp ../../images/sqlite-icon.png linux/usr/share/icons/hicolor/256x256/apps/sqlitequery.png

    if ($Install) {
        mkdir -p ~/.local/bin
        cp -rf ./linux /tmp/sqlitequery
        ln -s /tmp/sqlitequery/bin/SQLiteQueryAnalyzer ~/.local/bin/sqlitequery
    }

    if ($Package) {
        cpack -G 7Z --config ./build/CPackConfig.cmake
        cpack -G ZIP --config ./build/CPackConfig.cmake
        cpack -G TBZ2 --config ./build/CPackConfig.cmake
        cpack -G TGZ --config ./build/CPackConfig.cmake
        cpack -G TXZ --config ./build/CPackConfig.cmake
        cpack -G TZ --config ./build/CPackConfig.cmake
        cpack -G DEB --config ./build/CPackConfig.cmake
        cpack -G RPM --config ./build/CPackConfig.cmake
        snapcraft
    }
}

if ($IsMacOS -And $Package) {
    
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/sqlitequery
    cmake --build build --config Release --parallel 32
    if ($Package) {
        macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
    }
}
