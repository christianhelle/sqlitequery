param (
    [Parameter(Mandatory = $false)]
    [string]$BuildDir = ".\build\Release",

    [Parameter(Mandatory = $false)]
    [string]$OutputDir = "..\artifacts",

    [Parameter(Mandatory = $false)]
    [string]$Version = "0.2.0.0",

    [Parameter(Mandatory = $false)]
    [string]$SignCert = "",

    [Parameter(Mandatory = $false)]
    [string]$SignCertPassword = ""
)

$ErrorActionPreference = "Stop"

Write-Host "=== SQLite Query Analyzer MSIX Package Builder ===" -ForegroundColor Cyan

# Validate build directory exists
if (-not (Test-Path $BuildDir)) {
    Write-Error "Build directory not found at: $BuildDir"
    Write-Error "Please build the application first using build.ps1"
    exit 1
}

if (-not (Test-Path "$BuildDir\SQLiteQueryAnalyzer.exe")) {
    Write-Error "SQLiteQueryAnalyzer.exe not found in: $BuildDir"
    exit 1
}

# Create staging directory for MSIX contents
$stagingDir = ".\build\msix-staging"
if (Test-Path $stagingDir) {
    Remove-Item $stagingDir -Recurse -Force
}
New-Item -ItemType Directory -Path $stagingDir -Force | Out-Null
New-Item -ItemType Directory -Path $stagingDir\Assets -Force | Out-Null

Write-Host "Staging directory: $stagingDir"

# Copy application files
Write-Host "Copying application files..."
Copy-Item "$BuildDir\*" $stagingDir -Recurse -Force

# Copy MSIX assets
Write-Host "Copying MSIX assets..."
Copy-Item ".\msix\Assets\*" "$stagingDir\Assets\" -Force

# Copy and update AppxManifest.xml with the correct version
Write-Host "Preparing AppxManifest.xml with version $Version..."
$manifest = Get-Content ".\msix\AppxManifest.xml" -Raw
$manifest = $manifest -replace 'Version="[^"]*"', "Version=`"$Version`""
$manifest | Set-Content "$stagingDir\AppxManifest.xml" -Encoding UTF8

# Find MakeAppx.exe from Windows SDK
$sdkDir = "C:\Program Files (x86)\Windows Kits\10\bin"
$makeAppx = $null
$signTool = $null

if (Test-Path $sdkDir) {
    # Find the latest SDK version
    $sdkVersions = Get-ChildItem $sdkDir -Directory | Where-Object { $_.Name -match '^\d+\.' } | Sort-Object Name -Descending
    foreach ($sdkVersion in $sdkVersions) {
        $candidatePath = Join-Path $sdkVersion.FullName "x64\makeappx.exe"
        if (Test-Path $candidatePath) {
            $makeAppx = $candidatePath
            $signTool = Join-Path $sdkVersion.FullName "x64\signtool.exe"
            Write-Host "Found Windows SDK: $($sdkVersion.Name)"
            break
        }
    }
}

if (-not $makeAppx) {
    Write-Error "MakeAppx.exe not found. Please install the Windows 10/11 SDK."
    Write-Error "Download from: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/"
    exit 1
}

Write-Host "Using MakeAppx: $makeAppx"

# Create output directory
New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null

# Build MSIX package
$msixPath = Join-Path $OutputDir "SQLiteQueryAnalyzer.msix"
Write-Host "Creating MSIX package..."
& $makeAppx pack /d $stagingDir /p $msixPath /o
if ($LASTEXITCODE -ne 0) {
    Write-Error "MakeAppx.exe failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}

Write-Host "MSIX package created: $msixPath" -ForegroundColor Green

# Sign the package if a certificate is provided
if ($SignCert -and (Test-Path $SignCert)) {
    if (-not (Test-Path $signTool)) {
        Write-Error "SignTool.exe not found in Windows SDK"
        exit 1
    }

    Write-Host "Signing MSIX package..."
    $signArgs = @("sign", "/fd", "SHA256", "/f", $SignCert)
    if ($SignCertPassword) {
        $signArgs += "/p"
        $signArgs += $SignCertPassword
    }
    $signArgs += "/td"
    $signArgs += "SHA256"
    $signArgs += $msixPath

    & $signTool @signArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "SignTool.exe failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }
    Write-Host "MSIX package signed successfully" -ForegroundColor Green
} else {
    Write-Host "No signing certificate provided. Package is unsigned." -ForegroundColor Yellow
    Write-Host "For Microsoft Store submission, the package will be signed by Microsoft during certification." -ForegroundColor Yellow
    Write-Host "For sideloading, sign with: signtool sign /fd SHA256 /f cert.pfx /p password $msixPath" -ForegroundColor Yellow
}

# Clean up staging directory
Remove-Item $stagingDir -Recurse -Force

Write-Host ""
Write-Host "=== MSIX packaging complete ===" -ForegroundColor Cyan
Write-Host "Package: $msixPath"
Write-Host "Size: $([math]::Round((Get-Item $msixPath).Length / 1MB, 2)) MB"
