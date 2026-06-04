#Requires -Version 5.1

param(
  [string]$InstallDir = "",
  [switch]$Help
)

$GitHubRepo = "christianhelle/sqlitequery"
$BinaryName = "SQLiteQueryAnalyzer.exe"

function Write-ColorOutput {
  param([string]$Message, [string]$Color = "White", [string]$Emoji = "")
  if ($Emoji) { Write-Host "$Emoji " -NoNewline }
  Write-Host $Message -ForegroundColor $Color
}

function Write-Info { param([string]$Message) Write-ColorOutput -Message $Message -Color "Cyan" -Emoji "i" }
function Write-Success { param([string]$Message) Write-ColorOutput -Message $Message -Color "Green" -Emoji "+" }
function Write-InstallWarning { param([string]$Message) Write-ColorOutput -Message $Message -Color "Yellow" -Emoji "!" }
function Write-InstallError { param([string]$Message) Write-ColorOutput -Message $Message -Color "Red" -Emoji "x" }

function Get-DefaultInstallDir {
  return "${env:ProgramFiles}\SQLiteQueryAnalyzer"
}

function Test-IsInPath {
  param([string]$Directory)
  $pathDirs = $env:PATH -split ';' | ForEach-Object { $_.Trim('"').TrimEnd('\') }
  return $pathDirs -contains $Directory.TrimEnd('\')
}

function Add-ToUserPath {
  param([string]$Directory)
  if (Test-IsInPath -Directory $Directory) {
    Write-Info "Directory already in PATH: $Directory"
    return
  }
  try {
    $userPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    if (-not $userPath.EndsWith(';')) { $userPath += ';' }
    $newPath = $userPath + $Directory
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
    $env:PATH += ";$Directory"
    Write-Success "Added to user PATH: $Directory"
    Write-InstallWarning "Restart your terminal for PATH changes to take effect"
  } catch {
    Write-InstallError "Failed to add directory to PATH: $($_.Exception.Message)"
  }
}

function Get-LatestRelease {
  Write-Info "Fetching latest release information..."
  try {
    $apiUrl = "https://api.github.com/repos/$GitHubRepo/releases/latest"
    $response = Invoke-RestMethod -Uri $apiUrl -ErrorAction Stop
    return $response.tag_name
  } catch {
    Write-InstallError "Failed to fetch release information: $($_.Exception.Message)"
    throw
  }
}

function Get-AssetUrl {
  param([string]$Version)
  Write-Info "Fetching release assets for $Version..."
  try {
    # Use the release tag endpoint so the asset corresponds to the requested version
    $apiUrl = "https://api.github.com/repos/$GitHubRepo/releases/tags/$Version"
    $response = Invoke-RestMethod -Uri $apiUrl -ErrorAction Stop
    foreach ($asset in $response.assets) {
      if ($asset.name -like "*Windows*Installer*.exe") { return $asset.browser_download_url }
    }
    Write-InstallError "Asset not found for Windows installer"
    Write-Info "Available assets:"
    foreach ($asset in $response.assets) { Write-Info "  - $($asset.name)" }
    throw "Required asset not found"
  } catch {
    Write-InstallError "Failed to find release asset: $($_.Exception.Message)"
    throw
  }
}

function Install-SQLiteQueryAnalyzer {
  param([string]$Version, [string]$TargetDir)
  $downloadUrl = Get-AssetUrl -Version $Version
  $tempDir = Join-Path $env:TEMP "sqlitequery-install-$(Get-Random)"
  $installerPath = Join-Path $tempDir "SQLiteQueryAnalyzer-Setup.exe"
  try {
    New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
    Write-Info "Downloading SQLiteQueryAnalyzer $Version..."
    Invoke-WebRequest -Uri $downloadUrl -OutFile $installerPath -ErrorAction Stop
    Write-Info "Running installer..."
    $installerArgs = @(
      "/VERYSILENT",
      "/SUPPRESSMSGBOXES",
      "/NORESTART",
      "/DIR=`"$TargetDir`""
    )
    $process = Start-Process -FilePath $installerPath -ArgumentList $installerArgs -Wait -PassThru
    if ($process.ExitCode -ne 0) {
      throw "Installer exited with code: $($process.ExitCode)"
    }
    Write-Success "SQLiteQueryAnalyzer $Version installed successfully!"
    return (Join-Path $TargetDir $BinaryName)
  } catch {
    Write-InstallError "Installation failed: $($_.Exception.Message)"
    throw
  } finally {
    if (Test-Path $tempDir) { Remove-Item -Path $tempDir -Recurse -Force }
  }
}

function Test-Installation {
  param([string]$BinaryPath)
  if (Test-Path $BinaryPath) {
    Write-Success "Installation verified: $BinaryPath"
    Write-Info "You can now run: $BinaryName"
    return $true
  }
  Write-InstallWarning "Could not verify installation"
  return $false
}

function Main {
  if ($Help) {
    Write-Host ""
    Write-Host "SQLiteQueryAnalyzer Installation Script for Windows" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage:" -ForegroundColor Yellow
    Write-Host "  Invoke-RestMethod https://christianhelle.com/sqlitequery/install.ps1 | Invoke-Expression" -ForegroundColor White
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -InstallDir <path>   Installation directory (optional)" -ForegroundColor White
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  Invoke-RestMethod https://christianhelle.com/sqlitequery/install.ps1 -OutFile install.ps1; .\install.ps1" -ForegroundColor Gray
    Write-Host "  Invoke-RestMethod https://christianhelle.com/sqlitequery/install.ps1 -OutFile install.ps1; .\install.ps1 -InstallDir 'C:\MyApps'" -ForegroundColor Gray
    return
  }

  Write-Info "Starting SQLiteQueryAnalyzer installation for Windows..."

  if (-not $InstallDir) { $InstallDir = Get-DefaultInstallDir }
  Write-Info "Target directory: $InstallDir"

  try {
    $version = Get-LatestRelease
    Write-Info "Latest version: $version"
    $binaryPath = Install-SQLiteQueryAnalyzer -Version $version -TargetDir $InstallDir
    $ok = $false
    try {
        $ok = Test-Installation -BinaryPath $binaryPath
    } catch {
        $ok = $false
    }

    if ($ok) {
        Write-Host ""
        Write-Success "Installation complete!"
        Write-Info "You can launch SQLiteQueryAnalyzer from the Start Menu"
    } else {
        Write-InstallError "Installation verification failed"
        exit 1
    }
  } catch {
    Write-InstallError "Installation failed: $($_.Exception.Message)"
    exit 1
  }
}

Main
