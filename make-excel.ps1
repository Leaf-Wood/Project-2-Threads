param([string]$ResultsFolder)

$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot
if (-not $ResultsFolder) {
    if (Test-Path -LiteralPath 'results/linux/averages.csv') { $ResultsFolder = 'results/linux' }
    else { $ResultsFolder = 'results/windows' }
}
$folder = (Resolve-Path -LiteralPath $ResultsFolder).Path
$averages = @(Import-Csv -LiteralPath (Join-Path $folder 'averages.csv'))
$counts = @(Import-Csv -LiteralPath (Join-Path $folder 'histogram.csv') -Header Step,Frequency)
$machine = Split-Path $folder -Leaf
if ($machine -notin @('windows', 'linux')) { throw 'Use results/windows or results/linux.' }
$output = Join-Path $PSScriptRoot "results/$machine.xlsx"
if ($averages.Count -ne 16 -or $counts.Count -ne 1001) { throw 'Expected 16 averages and 1001 histogram rows.' }

# Create a column chart from the table on a worksheet.
function Add-Chart($sheet, $lastRow, $lastColumn, $title, $xTitle, $yTitle) {
    $chart = $sheet.ChartObjects().Add(280, 15, 650, 350).Chart
    $chart.ChartType = 51 # Clustered columns
    for ($column = 2; $column -le $lastColumn; $column++) {
        $series = $chart.SeriesCollection().NewSeries()
        $series.Name = $sheet.Cells.Item(1, $column).Value2
        $series.XValues = $sheet.Range($sheet.Cells.Item(2, 1), $sheet.Cells.Item($lastRow, 1))
        $series.Values = $sheet.Range($sheet.Cells.Item(2, $column), $sheet.Cells.Item($lastRow, $column))
    }
    $chart.HasTitle = $true
    $chart.ChartTitle.Text = $title
    $chart.Axes(1).HasTitle = $true
    $chart.Axes(1).AxisTitle.Text = $xTitle
    $chart.Axes(2).HasTitle = $true
    $chart.Axes(2).AxisTitle.Text = $yTitle
    $chart.Axes(2).MinimumScale = 0
    $chart.HasLegend = ($lastColumn -gt 2)
    return $chart
}

$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$excel.DisplayAlerts = $false
$book = $null
try {
    $book = $excel.Workbooks.Add(-4167) # One blank worksheet
    $runtime = $book.Worksheets.Item(1)
    $runtime.Name = 'Runtime'
    $histogram = $book.Worksheets.Add()
    $histogram.Name = 'Histogram'
    $runtime.Move($histogram)

    # Put locked and unlocked averages side by side.
    $table = New-Object 'object[,]' 9,3
    $table[0,0] = 'Threads'; $table[0,1] = 'Locked (s)'; $table[0,2] = 'NoLock (s)'
    for ($t = 1; $t -le 8; $t++) {
        $locked = @($averages | Where-Object { [int]$_.T -eq $t -and $_.Mode -eq 'Locked' })
        $unlocked = @($averages | Where-Object { [int]$_.T -eq $t -and $_.Mode -eq 'NoLock' })
        if ($locked.Count -ne 1 -or $unlocked.Count -ne 1) { throw "Missing or duplicate averages for T=$t" }
        $table[$t,0] = [double]$t
        $table[$t,1] = [double]$locked[0].AverageSeconds
        $table[$t,2] = [double]$unlocked[0].AverageSeconds
    }
    $runtime.Range('A1:C9').Value2 = $table
    $runtime.Range('B2:C9').NumberFormat = '0.000000'
    $null = Add-Chart $runtime 9 3 'Mean runtime by thread count' 'Threads' 'Seconds'

    # Import every stopping-time bin, including zeros.
    $table = New-Object 'object[,]' 1002,2
    $table[0,0] = 'Stopping time'; $table[0,1] = 'Frequency'
    for ($i = 0; $i -lt $counts.Count; $i++) {
        if ([int]$counts[$i].Step -ne $i) { throw 'Histogram bins are not in order.' }
        $table[($i+1),0] = [double]$counts[$i].Step
        $table[($i+1),1] = [double]$counts[$i].Frequency
    }
    $histogram.Range('A1:B1002').Value2 = $table
    $histogram.Range('B2:B1002').NumberFormat = '#,##0'
    $chart = Add-Chart $histogram 1002 2 'Collatz stopping time histogram' 'Stopping time (steps)' 'Frequency'
    $chart.Axes(1).TickLabelSpacing = 100
    $chart.ChartGroups(1).GapWidth = 0

    foreach ($sheet in @($runtime, $histogram)) {
        $sheet.UsedRange.Columns.AutoFit() | Out-Null
        $sheet.UsedRange.Rows.Item(1).Font.Bold = $true
    }
    # Keep the machine details recorded for this run.
    $system = $book.Worksheets.Add()
    $system.Name = 'System'
    $system.Cells.Item(1, 1).Value2 = 'System specifications for this run'
    $row = 3
    foreach ($line in Get-Content -LiteralPath (Join-Path $folder 'system-specs.txt')) {
        if ($line.Trim()) {
            $system.Cells.Item($row, 1).Value2 = $line.Trim()
            $row++
        }
    }
    $row += 2
    $system.Cells.Item($row, 1).Value2 = 'AI disclosure: AI assisted with this Excel helper. Measurements come from the program.'
    $system.Columns.Item(1).AutoFit() | Out-Null
    $system.Cells.Item(1, 1).Font.Bold = $true

    $runtime.Activate()
    $book.SaveAs($output, 51) # Standard .xlsx workbook
    Write-Host "Created $output"
}
finally {
    if ($null -ne $book) { $book.Close($false) }
    $excel.Quit()
    [void][Runtime.InteropServices.Marshal]::ReleaseComObject($excel)
}


