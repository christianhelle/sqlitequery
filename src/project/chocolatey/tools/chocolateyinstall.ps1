$sqlitequeryDir = "$env:ALLUSERSPROFILE\chocolatey\lib\sqlitequery"
# $sqlitequeryExe = "$sqlitequeryDir\tools\sqlitequery.exe"
# Install-ChocolateyFileAssociation ".sdf" $sqlitequeryExe

cmd /c assoc .sqlite=sqlitedbfile
cmd /c ftype sqlitedbfile=sqlitequery.exe -File `"sqlitequery.exe`" `"%1`"

$WshShell = New-Object -comObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut("$Home\Desktop\SQLite Query Analyzer.lnk")
$Shortcut.TargetPath = "$sqlitequeryDir\tools\sqlitequery.exe"
$Shortcut.Save()
