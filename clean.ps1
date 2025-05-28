$basePath = '.\build'

# Get all subdirectories recursively inside .\build
$subDirs = Get-ChildItem -Path $basePath -Directory -Recurse | Select-Object -ExpandProperty FullName

# Base patterns to delete in the base directory
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
        Write-Host "[MAKE] Files found matching pattern: [$pattern] -> Delete"
        Remove-Item $files.FullName -Force
    } else {
        Write-Host "[MAKE] No files found matching pattern: [$pattern] -> Skip"
    }
}
