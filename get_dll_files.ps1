[string]$sourceDirectory  = $env:OPENCV_DIR + "\bin\Debug\opencv_world451d.dll"
[string]$destinationDirectory = $PSScriptRoot + "\MonitoringCPR\MonitoringCPR"
Copy-item -Force -Recurse -Verbose $sourceDirectory -Destination $destinationDirectory

[string]$sourceDirectory  = $env:OPENCV_DIR + "\bin\Release\opencv_world451.dll"
[string]$destinationDirectory = $PSScriptRoot + "\MonitoringCPR\MonitoringCPR"
Copy-item -Force -Recurse -Verbose $sourceDirectory -Destination $destinationDirectory

[string]$sourceDirectory  = $env:OPENCV_DIR + "\bin\Debug\opencv_world451d.dll"
[string]$destinationDirectory = $PSScriptRoot + "\MonitoringCPR\MonitoringCPR"
Copy-item -Force -Recurse -Verbose $sourceDirectory -Destination $destinationDirectory

[string]$sourceDirectory  = $env:OPENCV_DIR + "\bin\Release\opencv_world451.dll"
[string]$destinationDirectory = $PSScriptRoot + "\MonitoringCPR\MonitoringCPR"
Copy-item -Force -Recurse -Verbose $sourceDirectory -Destination $destinationDirectory

[string]$sourceDirectory  = $env:OPENCV_DIR + "\bin\Debug\opencv_world451d.dll"
[string]$destinationDirectory = $PSScriptRoot + "\MonitoringCPRUnityProject\Assets\Libraries"
Copy-item -Force -Recurse -Verbose $sourceDirectory -Destination $destinationDirectory

[string]$sourceDirectory  = $env:OPENCV_DIR + "\bin\Release\opencv_world451.dll"
[string]$destinationDirectory = $PSScriptRoot + "\MonitoringCPRUnityProject\Assets\Libraries"
Copy-item -Force -Recurse -Verbose $sourceDirectory -Destination $destinationDirectory