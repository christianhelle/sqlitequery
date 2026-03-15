#Requires -Version 5.1

<#
.SYNOPSIS
    SQLiteQueryAnalyzer - Installation Script for Windows

.DESCRIPTION
    This script downloads and installs the latest release of SQLiteQueryAnalyzer for Windows.

.PARAMETER InstallDir
    The directory to install SQLiteQueryAnalyzer to. Defaults to a directory in the user's PATH.

.PARAMETER AddToPath
    Whether to add the installation directory to the user's PATH. Default is $true.

.PARAMETER Force
    Force installation even if SQLiteQueryAnalyzer is already installed.

.EXAMPLE
    # Install using default settings
    irm https://christianhelle.com/sqlitequery/install.ps1 | iex

.EXAMPLE
    # Install to a specific directory
    irm https://christianhelle.com/sqlitequery/install.ps1 | iex -InstallDir "C:\Tools"

.EXAMPLE
    # Install without adding to PATH
    irm https://christianhelle.com/sqlitequery/install.ps1 | iex -AddToPath $false
#>

param(
  [string]$InstallDir = "",
  [bool]$AddToPath = $true,
  [switch]$Force
)

# Configuration
$GitHubRepo = "christianhelle/sqlitequery"
$BinaryName = "SQLiteQueryAnalyzer.exe"
$ArchiveName = "SQLiteQuerAnalyzer for Windows (Binaries)"

# Function to write colored output
function Write-ColorOutput
{
  param(
    [string]$Message,
    [string]$Color = "White",
    [string]$Emoji = ""
  )
    
  if ($Emoji)
  {
    Write-Host "$Emoji " -NoNewline
  }
  Write-Host $Message -ForegroundColor $Color
}

function Write-Info
{
  param([string]$Message)
  Write-ColorOutput -Message $Message -Color "Cyan" -Emoji "ℹ️"
}

function Write-Success
{
  param([string]$Message)
  Write-ColorOutput -Message $Message -Color "Green" -Emoji "✅"
}

function Write-Warning
{
  param([string]$Message)
  Write-ColorOutput -Message $Message -Color "Yellow" -Emoji "⚠️"
}

function Write-Error
{
  param([string]$Message)
  Write-ColorOutput -Message $Message -Color "Red" -Emoji "❌"
}

function Show-Usage
{
  Write-Host ""
  Write-Host "SQLiteQueryAnalyzer Installation Script for Windows" -ForegroundColor "Blue"
  Write-Host ""
  Write-Host "Usage:" -ForegroundColor "Yellow"
  Write-Host "  irm https://christianhelle.com/sqlitequery/install.ps1 | iex" -ForegroundColor "White"
  Write-Host ""
  Write-Host "Parameters:" -ForegroundColor "Yellow"
  Write-Host "  -InstallDir <path>   Installation directory" -ForegroundColor "White"
  Write-Host "  -AddToPath <bool>    Add to PATH (default: true)" -ForegroundColor "White"
  Write-Host "  -Force              Force installation" -ForegroundColor "White"
  Write-Host ""
  Write-Host "Examples:" -ForegroundColor "Yellow"
  Write-Host "  # Default installation" -ForegroundColor "Gray"
  Write-Host "  irm https://christianhelle.com/sqlitequery/install.ps1 | iex" -ForegroundColor "White"
  Write-Host ""
  Write-Host "  # Custom directory" -ForegroundColor "Gray"
  Write-Host "  irm https://christianhelle.com/sqlitequery/install.ps1 | iex -InstallDir 'C:\Tools'" -ForegroundColor "White"
  Write-Host ""
}

function Get-DefaultInstallDir
{
  $candidates = @(
    "$env:LOCALAPPDATA\Programs\SQLiteQueryAnalyzer",
    "$env:USERPROFILE\.local\bin",
    "$env:USERPROFILE\bin"
  )
    
  foreach ($candidate in $candidates)
  {
    if (Test-Path $candidate -PathType Container)
    {
      return $candidate
    }
  }
    
  return "$env:LOCALAPPDATA\Programs\SQLiteQueryAnalyzer"
}

function Test-IsInPath
{
  param([string]$Directory)
    
  $pathDirs = $env:PATH -split ';' | ForEach-Object { $_.Trim('"').TrimEnd('\') }
  $targetDir = $Directory.TrimEnd('\')
    
  return $pathDirs -contains $targetDir
}

function Add-ToUserPath
{
  param([string]$Directory)
    
  if (Test-IsInPath -Directory $Directory)
  {
    Write-Info "Directory already in PATH: $Directory"
    return
  }
    
  try
  {
    $userPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    if (-not $userPath.EndsWith(';'))
    {
      $userPath += ';'
    }
    $newPath = $userPath + $Directory
        
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
        
    $env:PATH += ";$Directory"
        
    Write-Success "Added to user PATH: $Directory"
    Write-Warning "Restart your terminal for PATH changes to take effect"
  } catch
  {
    Write-Error "Failed to add directory to PATH: $($_.Exception.Message)"
    Write-Info "You can manually add '$Directory' to your PATH"
  }
}

function Get-LatestRelease
{
  Write-Info "Fetching latest release information..."
    
  try
  {
    $apiUrl = "https://api.github.com/repos/$GitHubRepo/releases/latest"
    $response = Invoke-RestMethod -Uri $apiUrl -ErrorAction Stop
    return $response.tag_name
  } catch
  {
    Write-Error "Failed to fetch release information: $($_.Exception.Message)"
    throw
  }
}

function Get-AssetUrl
{
  param([string]$Version)
  
  Write-Info "Fetching release assets..."
  
  try
  {
    $apiUrl = "https://api.github.com/repos/$GitHubRepo/releases/latest"
    $response = Invoke-RestMethod -Uri $apiUrl -ErrorAction Stop
    
    $archiveName = "$ArchiveName v$Version.zip"
    
    foreach ($asset in $response.assets)
    {
      if ($asset.name -eq $archiveName)
      {
        return $asset.browser_download_url
      }
    }
    
    Write-Error "Asset not found: $archiveName"
    Write-Info "Available assets:"
    foreach ($asset in $response.assets)
    {
      Write-Info "  - $($asset.name)"
    }
    throw "Required asset not found"
  } catch
  {
    Write-Error "Failed to find release asset: $($_.Exception.Message)"
    throw
  }
}

function Test-SQLiteQueryAnalyzerInstalled
{
  try
  {
    $version = & $BinaryName --version 2>$null
    if ($LASTEXITCODE -eq 0)
    {
      return $version
    }
  } catch
  {
  }
  return $null
}

function Install-SQLiteQueryAnalyzer
{
  param(
    [string]$Version,
    [string]$TargetDir
  )
    
  $downloadUrl = Get-AssetUrl -Version $Version
  $tempDir = Join-Path $env:TEMP "sqlitequery-install-$(Get-Random)"
  $archivePath = Join-Path $tempDir "sqlitequery.zip"
    
  try
  {
    New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        
    Write-Info "Downloading SQLiteQueryAnalyzer $Version..."
    Invoke-WebRequest -Uri $downloadUrl -OutFile $archivePath -ErrorAction Stop
        
    Write-Info "Extracting archive..."
    Expand-Archive -Path $archivePath -DestinationPath $tempDir -Force
        
    if (-not (Test-Path $TargetDir))
    {
      Write-Info "Creating installation directory: $TargetDir"
      New-Item -ItemType Directory -Path $TargetDir -Force | Out-Null
    }
        
    Write-Info "Installing to $TargetDir..."
    
    # Find all exe files in the extracted directory
    $sourceFiles = Get-ChildItem -Path $tempDir -Filter "*.exe" -Recurse
    
    if ($sourceFiles.Count -eq 0)
    {
      throw "No executable found in archive"
    }
    
    foreach ($file in $sourceFiles)
    {
      $targetPath = Join-Path $TargetDir $file.Name
      Copy-Item -Path $file.FullName -Destination $targetPath -Force
    }
        
    Write-Success "SQLiteQueryAnalyzer $Version installed successfully!"
        
    return (Join-Path $TargetDir $BinaryName)
  } catch
  {
    Write-Error "Installation failed: $($_.Exception.Message)"
    throw
  } finally
  {
    if (Test-Path $tempDir)
    {
      Remove-Item -Path $tempDir -Recurse -Force
    }
  }
}

function Test-Installation
{
  param([string]$BinaryPath)
    
  try
  {
    $version = & $BinaryPath --version 2>$null
    if ($LASTEXITCODE -eq 0)
    {
      Write-Success "Installation verified: $version"
      Write-Info "You can now run: $BinaryName"
      return $true
    }
  } catch
  {
  }
    
  Write-Warning "Could not verify installation"
  return $false
}

function Main
{
  Write-Info "Starting SQLiteQueryAnalyzer installation for Windows..."
    
  # Check if already installed
  $existingVersion = Test-SQLiteQueryAnalyzerInstalled
  if ($existingVersion -and -not $Force)
  {
    Write-Warning "SQLiteQueryAnalyzer is already installed: $existingVersion"
    Write-Info "Use -Force to reinstall"
    return
  }
    
  if ($existingVersion)
  {
    Write-Warning "Updating from $existingVersion..."
  }
    
  # Determine installation directory
  if (-not $InstallDir)
  {
    $InstallDir = Get-DefaultInstallDir
  }
    
  Write-Info "Target directory: $InstallDir"
    
  try
  {
    $version = Get-LatestRelease
    Write-Info "Latest version: $version"
        
    $binaryPath = Install-SQLiteQueryAnalyzer -Version $version -TargetDir $InstallDir
        
    if ($AddToPath)
    {
      Add-ToUserPath -Directory $InstallDir
    }
        
    Test-Installation -BinaryPath $binaryPath
        
    Write-Host ""
    Write-Success "🎉 Installation complete!"
    Write-Info "Get started with: $BinaryName"
    Write-Info "Documentation: https://christianhelle.com/sqlitequery/"
        
    if ($AddToPath)
    {
      Write-Host ""
      Write-Warning "Note: You may need to restart your terminal for PATH changes to take effect"
    }
  } catch
  {
    Write-Error "Installation failed: $($_.Exception.Message)"
    exit 1
  }
}

Main
