param(
    [Parameter(Mandatory = $true)]
    [string]$SkillDir
)

$python = "C:\Users\2230258\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe"
$validator = "C:\Users\2230258\.codex\skills\.system\skill-creator\scripts\quick_validate.py"
$compatPath = Join-Path $PSScriptRoot "skill-creator-compat"
$previousPyPath = $env:PYTHONPATH

try {
    if ([string]::IsNullOrWhiteSpace($previousPyPath)) {
        $env:PYTHONPATH = $compatPath
    }
    else {
        $env:PYTHONPATH = "$compatPath;$previousPyPath"
    }

    & $python $validator $SkillDir
    exit $LASTEXITCODE
}
finally {
    $env:PYTHONPATH = $previousPyPath
}
