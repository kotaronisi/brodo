param(
    [Parameter(Mandatory = $true)]
    [string]$SkillDir,

    [Parameter(Mandatory = $true)]
    [string]$SkillName,

    [string[]]$Interface = @()
)

$outputPath = Join-Path $SkillDir "agents\openai.yaml"
$agentsDir = Split-Path -Parent $outputPath

$allowedKeys = @(
    "display_name",
    "short_description",
    "icon_small",
    "icon_large",
    "brand_color",
    "default_prompt"
)

$values = @{}
foreach ($item in $Interface) {
    $parts = $item -split "=", 2
    if ($parts.Count -ne 2) {
        throw "Invalid interface override '$item'. Use key=value."
    }

    $key = $parts[0].Trim()
    $value = $parts[1].Trim()
    if ($allowedKeys -notcontains $key) {
        throw "Unknown interface field '$key'."
    }
    $values[$key] = $value
}

if (-not $values.ContainsKey("display_name")) {
    $words = $SkillName -split "-" | Where-Object { $_ -ne "" }
    $values["display_name"] = ($words | ForEach-Object {
        if ($_.Length -eq 0) { $_ } else { $_.Substring(0, 1).ToUpper() + $_.Substring(1) }
    }) -join " "
}

if (-not $values.ContainsKey("short_description")) {
    $values["short_description"] = "Help with $($values['display_name']) tasks"
}

if ($values["short_description"].Length -lt 25 -or $values["short_description"].Length -gt 64) {
    throw "short_description must be 25-64 characters."
}

if (-not (Test-Path $agentsDir)) {
    New-Item -ItemType Directory -Path $agentsDir | Out-Null
}

function Quote-YamlString([string]$Value) {
    return '"' + $Value.Replace('\', '\\').Replace('"', '\"') + '"'
}

$orderedKeys = @(
    "display_name",
    "short_description",
    "icon_small",
    "icon_large",
    "brand_color",
    "default_prompt"
)

$lines = @("interface:")
foreach ($key in $orderedKeys) {
    if ($values.ContainsKey($key)) {
        $lines += "  ${key}: $(Quote-YamlString $values[$key])"
    }
}

$content = ($lines -join "`n") + "`n"
Set-Content -LiteralPath $outputPath -Value $content -Encoding UTF8
Write-Output "[OK] Created agents/openai.yaml"
