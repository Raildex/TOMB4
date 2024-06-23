param (
	[string]$SourceFile,
	[string]$OutputPath
)
$structs = Get-Content -Path $SourceFile -Raw | Select-String -List -Pattern "(?:struct|enum)\s*([a-zA-Z0-9_]+)\s*{(?:\s*.*?\s*)*?}\s*;" -AllMatches
foreach($struct in $structs.Matches) {
	Write-Output $struct.Value
	New-Item "$($OutputPath)/$($struct.groups[1].Value.ToLower().Replace('_','')).h" -Force -Value @"
#ifndef $($struct.groups[1].Value.ToUpper())_INCLUDED
#define $($struct.groups[1].Value.ToUpper())_INCLUDED
$($struct.Value)
#endif
"@
}