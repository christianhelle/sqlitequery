param (
    [switch]
    $Package = $false
)

if ($IsWindows) {
    cmake . -DCMAKE_PREFIX_PATH=C:/Qt/6.9.0/msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" -B build
    cmake --build build --config Release --parallel 32
    if ($Package) {
        C:\Qt\6.9.0\msvc2022_64\bin\windeployqt.exe .\build\Release\sqlitequery.exe
        ../../deps/innosetup/ISCC.exe setup.iss
    }
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

if ($IsMacOS -And $Package) {
    
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/sqlitequery
    cmake --build build --config Release --parallel 32
    if ($Package) {
        macdeployqt build/sqlitequery.app -dmg -appstore-compliant
    }
}
