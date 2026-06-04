$sqlitequeryDir = "$env:ALLUSERSPROFILE\chocolatey\lib\sqlitequery"
# $sqlitequeryExe = "$sqlitequeryDir\tools\SQLiteQueryAnalyzer.exe"
# Install-ChocolateyFileAssociation ".sdf" $sqlitequeryExe

cmd /c assoc .sqlite=sqlitedbfile
cmd /c ftype sqlitedbfile=SQLiteQueryAnalyzer.exe -File `"SQLiteQueryAnalyzer.exe`" `"%1`"

$WshShell = New-Object -comObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut("$Home\Desktop\SQLite Query Analyzer.lnk")
$Shortcut.TargetPath = "$sqlitequeryDir\tools\SQLiteQueryAnalyzer.exe"
$Shortcut.Save()
