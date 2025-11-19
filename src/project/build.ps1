param (
    [switch]
    $Package = $false
)

if ($IsWindows) {
    # Check if Ninja is installed
    if (-not (Get-Command ninja -ErrorAction SilentlyContinue)) {
        Write-Error "Ninja build system is not installed. Please install Ninja."
        Write-Error "Install via: choco install ninja (or download from https://github.com/ninja-build/ninja/releases)"
        exit 1
    }
    
    # Check if Visual Studio is installed (including preview versions) for compiler
    $vswherePath = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswherePath)) {
        Write-Error "vswhere.exe not found at $vswherePath"
        Write-Error "Please install Visual Studio 2022 with C++ development tools from: https://visualstudio.microsoft.com/downloads/"
        exit 1
    }
    
    # Check if Visual Studio is installed (including preview versions) for compiler
    $vsPath = & $vswherePath -latest -prerelease -property installationPath 2>$null
    if (-not $vsPath) {
        Write-Error "Visual Studio is not installed. Please install Visual Studio 2022 with C++ development tools."
        Write-Error "Download from: https://visualstudio.microsoft.com/downloads/"
        exit 1
    }
    
    Write-Host "Using Visual Studio at: $vsPath"
    
    # Use Ninja generator
    $generator = "Ninja"
    Write-Host "Using CMake generator: $generator"
    
    # Check if Qt is installed
    if (-not (Test-Path "C:\Qt\6.9.0\msvc2022_64")) {
        Write-Error "Qt 6.9.0 MSVC2022 64-bit is not installed at C:\Qt\6.9.0\msvc2022_64"
        Write-Error "Please install Qt 6.9.0 for MSVC 2022 64-bit from: https://www.qt.io/download"
        exit 1
    }
    
    # Find vcvars64.bat to setup Visual Studio environment
    $vcvars = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
    if (-not (Test-Path $vcvars)) {
        Write-Error "Visual Studio C++ tools not found. Please install C++ development workload."
        exit 1
    }
    
    # Setup Visual Studio environment and run CMake with Ninja generator
    & cmd /c "`"$vcvars`" && cmake . -G `"$generator`" -DCMAKE_PREFIX_PATH=C:\Qt\6.9.0\msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS=`"/Zc:__cplusplus /permissive-`" -DCMAKE_BUILD_TYPE=Release -B build"
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configuration failed"
        exit $LASTEXITCODE
    }
    
    & cmd /c "`"$vcvars`" && cmake --build build --parallel 32"
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed"
        exit $LASTEXITCODE
    }

    New-Item -ItemType Directory -Path .\build\Release -Force
    Copy-Item .\build\SQLiteQueryAnalyzer.exe .\build\Release\SQLiteQueryAnalyzer.exe
    C:\Qt\6.9.0\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe
    
    if ($Package) {
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
        macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
    }
}
