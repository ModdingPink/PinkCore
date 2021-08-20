$files = Get-Childitem ./src -Recurse

foreach ($file in $files)
{
    if ($file.Mode.Contains('d'))
    {
        continue
    }

    $content = Get-Content $file | Select-String "manual"
    if ($content)
    {
        echo $file
    }
}