$basePath = '.\build'

# Get all immediate subdirectories inside .\build
$subDirs = Get-ChildItem -Path $basePath -Directory | Select-Object -ExpandProperty FullName

# Base patterns to delete
$patterns = @(
    "$basePath\kernel8.elf",
    "$basePath\*.o",
    "*.img"
)

# Add patterns for all subdirectories found, deleting *.o files in each
foreach ($dir in $subDirs) {
    $patterns += Join-Path $dir '*.o'
}

foreach ($pattern in $patterns) {
    $files = Get-ChildItem -Path $pattern -ErrorAction SilentlyContinue
    if ($files) {
        Write-Host "[INFO] Files found matching pattern: [$pattern] -> Delete"
        Remove-Item $files.FullName -Force
    } else {
        Write-Host "[INFO] No files found matching pattern: [$pattern] -> Skip"
    }
}

