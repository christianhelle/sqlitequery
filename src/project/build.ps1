param (
    [switch]
    $Package = $false
)

if ($IsWindows) {
    cmake . -DCMAKE_PREFIX_PATH=C:/Qt/6.8.2/msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" -B build
    cmake --build build --config Release --parallel 32
}

if ($IsLinux) {
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./linux/
    cmake --build build --config Release --parallel $(nproc)
    cmake --install build

    if ($Package) {
        cpack -G 7Z --config ./build/CPackConfig.cmake
        cpack -G ZIP --config ./build/CPackConfig.cmake
        cpack -G TBZ2 --config ./build/CPackConfig.cmake
        cpack -G TGZ --config ./build/CPackConfig.cmake
        cpack -G TXZ --config ./build/CPackConfig.cmake
        cpack -G TZ --config ./build/CPackConfig.cmake
        cpack -G DEB --config ./build/CPackConfig.cmake
        cpack -G RPM --config ./build/CPackConfig.cmake

        if (Get-Command snapcraft -ErrorAction SilentlyContinue) {
            snapcraft
        } else {
            Write-Warning "snapcraft not found. Snap package will not be created."
            Write-Warning "To install snapcraft, run: sudo snap install snapcraft --classic"
        }
    }
}

if ($IsWindows -And $package) {
    C:\Qt\6.8.2\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe

    (Get-FileHash build/Release/SQLiteQueryAnalyzer.exe -Algorithm SHA1 | Select-Object).Hash 
    | Set-Content build/Release/SQLiteQueryAnalyzer.exe.sha1

    (Get-FileHash build/Release/SQLiteQueryAnalyzer.exe -Algorithm MD5 | Select-Object).Hash 
    | Set-Content build/Release/SQLiteQueryAnalyzer.exe.md5

    ../../deps/innosetup/ISCC.exe setup.iss
}

if ($IsMacOS -And $Package) {
    
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/sqlitequery
    cmake --build build --config Release --parallel 32
    if ($Package) {
        macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
    }
}
