# Publishing SQLite Query Analyzer to the Microsoft Store

This guide covers the manual steps required to publish SQLite Query Analyzer to the Microsoft Store as an MSIX package.

## Overview

The repository includes automated tooling to build MSIX packages. However, several steps must be completed manually through Microsoft Partner Center.

## Prerequisites

Before you begin, ensure you have:

- A Microsoft account (personal or organizational)
- The Windows 10/11 SDK installed (for `MakeAppx.exe` and `SignTool.exe`)
- The application built successfully on Windows (see main README)

## Step 1: Create a Microsoft Developer Account

1. Navigate to [storedeveloper.microsoft.com](https://storedeveloper.microsoft.com)
2. Click **"Get started for free"**
3. Select **Individual developer** (free for individuals) or **Company account** ($99 USD one-time fee)
4. Sign in with your Microsoft account or create a new one
5. Complete identity verification (government-issued ID + selfie for individual accounts)
6. Complete your profile details
7. Click **"Go to Partner Center dashboard"**

After setup, you'll land on the **Apps & Games overview** page in [Partner Center](https://partner.microsoft.com/dashboard).

> **Note:** If the Apps & Games tile doesn't appear immediately, wait ~5 minutes and refresh.

## Step 2: Reserve Your App Name

1. Go to the [Partner Center apps and games page](https://aka.ms/submitwindowsapp)
2. Click **New product** → **MSIX or PWA app**
3. Enter **"SQLite Query Analyzer"** and click **Check availability**
4. If available, click **Reserve product name**

> Names are reserved for 3 months. If not used, the reservation expires.

## Step 3: Get Your Package Identity

After reserving the name, Partner Center assigns your package identity values:

1. In Partner Center, go to **Apps** → **SQLite Query Analyzer** → **Product Identity**
2. Note the following values:
   - **Package/Identity/Name** (e.g., `12345CompanyName.SQLiteQueryAnalyzer`)
   - **Package/Identity/Publisher** (e.g., `CN=XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX`)
   - **Package/Properties/PublisherDisplayName**

3. Update `src/project/msix/AppxManifest.xml` with these values:
   ```xml
   <Identity
     Name="YOUR_PACKAGE_IDENTITY_NAME"
     Version="0.2.0.0"
     Publisher="YOUR_PUBLISHER_IDENTITY"
     ProcessorArchitecture="x64" />

   <Properties>
     <DisplayName>SQLite Query Analyzer</DisplayName>
     <PublisherDisplayName>YOUR_PUBLISHER_DISPLAY_NAME</PublisherDisplayName>
     ...
   </Properties>
   ```

> **Important:** The `Name` and `Publisher` in the manifest **must exactly match** the values from Partner Center.

## Step 4: Build the MSIX Package

### Option A: Using the build script directly

```powershell
cd src/project
# Build the application first
pwsh build.ps1
# Then build the MSIX package
pwsh build-msix.ps1 -Version "0.2.0.0"
```

### Option B: Using build.ps1 with the -Msix flag

```powershell
cd src/project
pwsh build.ps1 -Msix -Version "0.2.0.0"
```

### Option C: Using CI/CD

The GitHub Actions release workflow automatically builds the MSIX package. Download it from the workflow artifacts.

The MSIX package will be created at `src/artifacts/SQLiteQueryAnalyzer.msix`.

## Step 5: Code Signing (Optional for Store Submission)

For **Microsoft Store distribution**, you do NOT need to sign the package yourself — Microsoft signs it during certification.

For **sideloading** (distributing outside the Store), you need to sign the package:

### Create a self-signed certificate (for testing)

```powershell
# Create a self-signed certificate (must match Publisher in AppxManifest.xml)
New-SelfSignedCertificate `
  -Type Custom `
  -Subject "CN=Christian Resma Helle" `
  -KeyUsage DigitalSignature `
  -FriendlyName "SQLiteQueryAnalyzer Dev Certificate" `
  -CertStoreLocation "Cert:\CurrentUser\My" `
  -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}")

# Export to PFX
$cert = Get-ChildItem "Cert:\CurrentUser\My" | Where-Object { $_.Subject -eq "CN=Christian Resma Helle" }
$password = ConvertTo-SecureString -String "YourPassword" -Force -AsPlainText
Export-PfxCertificate -Cert $cert -FilePath ".\dev-cert.pfx" -Password $password
```

### Sign the package

```powershell
pwsh build-msix.ps1 -Version "0.2.0.0" -SignCert ".\dev-cert.pfx" -SignCertPassword "YourPassword"
```

Or sign manually:

```powershell
signtool sign /fd SHA256 /a /f dev-cert.pfx /p YourPassword /td SHA256 SQLiteQueryAnalyzer.msix
```

> **Important:** Never commit certificates or passwords to source control. Use GitHub Actions secrets for CI/CD signing.

## Step 6: Submit to the Microsoft Store

1. Go to [Partner Center](https://partner.microsoft.com/dashboard) → **Apps** → **SQLite Query Analyzer**
2. Click **Start your submission** (or **Update** for subsequent versions)

### 6a. Packages

1. Upload the `SQLiteQueryAnalyzer.msix` file
2. Partner Center validates the package automatically

### 6b. Store Listing

Fill in the required Store listing information:

| Field | Suggested Value |
|-------|----------------|
| **Description** | A fast and lightweight cross-platform tool for querying and manipulating SQLite databases. Features include SQL query execution with syntax highlighting, table data browsing, CSV export, SQL script execution, and database schema analysis. |
| **Screenshots** | Use screenshots from the `images/` directory (minimum 1 required, recommend 3+). Must be 1366x768 or 2560x1440 for desktop. |
| **Category** | Developer Tools |
| **Search terms** | sqlite, database, query, sql, analyzer |
| **Privacy policy URL** | Link to your privacy policy (required if app accesses the internet) |
| **Website** | https://github.com/christianhelle/sqlitequery |
| **Support contact** | Your email or support URL |

### 6c. Pricing and Availability

- **Price**: Free (recommended) or set your price
- **Markets**: Select which countries/regions to publish in
- **Visibility**: Public (or Private for testing first)

### 6d. Properties

- **Category**: Developer Tools
- **System requirements**: Windows 10 version 1809 (build 17763) or later
- **Age rating**: Complete the age rating questionnaire (this app should qualify for "3+" / Everyone)

### 6e. Submit for Certification

1. Review all sections and ensure they're complete
2. Click **Submit to the Store**
3. Microsoft reviews the submission (typically 1-3 business days)
4. You'll receive an email when certification is complete

## Step 7: Post-Publication

### Updating the App

1. Increment the version in `AppxManifest.xml` (version must always increase)
2. Build a new MSIX package
3. In Partner Center, create a new submission with the updated package
4. Submit for certification

### Monitoring

- View download statistics, crash reports, and reviews in Partner Center
- Navigate to **Analytics** → **Overview** for insights

## MSIX Package Structure

The MSIX package contains:

```
SQLiteQueryAnalyzer.msix
├── AppxManifest.xml          # Package manifest
├── SQLiteQueryAnalyzer.exe   # Main executable
├── *.dll                     # Qt and dependency DLLs
├── Assets/
│   ├── Square44x44Logo.png   # Small app icon (44x44)
│   ├── Square150x150Logo.png # Medium tile (150x150)
│   ├── Wide310x150Logo.png   # Wide tile (310x150)
│   ├── LargeTile.png         # Large tile (310x310)
│   ├── SmallTile.png         # Small tile (71x71)
│   ├── StoreLogo.png         # Store logo (50x50)
│   └── SplashScreen.png      # Splash screen (620x300)
├── platforms/                # Qt platform plugins
├── styles/                   # Qt style plugins
└── ...                       # Other Qt deployment files
```

## Troubleshooting

### "Package validation error" during upload
- Ensure the `Identity Name` and `Publisher` in `AppxManifest.xml` match Partner Center values exactly
- Ensure the version number is in quad notation (e.g., `0.2.1.0`)
- Ensure all referenced asset files exist and are the correct sizes

### "MakeAppx.exe not found"
- Install the [Windows 10/11 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
- The tool is located at: `C:\Program Files (x86)\Windows Kits\10\bin\<version>\x64\makeappx.exe`

### App crashes after installation from MSIX
- Ensure all Qt dependencies are included (run `windeployqt.exe` before packaging)
- Check that the `vc_redist.x64.exe` is NOT included (the Visual C++ runtime should be installed separately or the user should already have it)

### Certification failure
- Review the certification report in Partner Center for specific issues
- Common issues: missing privacy policy, incorrect age rating, app crashes during testing
- See [Microsoft Store Policies](https://learn.microsoft.com/en-us/windows/uwp/publish/store-policies) for full requirements

## References

- [Publish your app to the Microsoft Store](https://learn.microsoft.com/en-us/windows/apps/publish/)
- [Package a desktop app manually (MSIX)](https://learn.microsoft.com/en-us/windows/msix/desktop/desktop-to-uwp-manual-conversion)
- [MakeAppx.exe tool](https://learn.microsoft.com/en-us/windows/msix/package/create-app-package-with-makeappx-tool)
- [MSIX Signing Overview](https://learn.microsoft.com/en-us/windows/msix/package/signing-package-overview)
- [Partner Center](https://partner.microsoft.com/dashboard)
