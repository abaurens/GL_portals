
function RenameEntry($file) {
	$fullPath=$file.FullName
	
	$NewName=$file.Name
	$NewName=$NewName.replace('__Project_Name__', $ProjectName)
	$NewName=$NewName.replace('__Workspace_Name__', $WorkspaceName)
	
	if ($file.Name -ne $NewName)
	{
		Write-Host "mv $fullPath $NewName"
		Rename-Item -Path $fullPath -NewName $NewName
	}
}

function ReplaceTokens($file) {
	Write-Host $file.FullName
	
	(Get-Content $file.FullName).Replace('__Workspace_Name__', "$WorkspaceName") | Set-Content -Encoding ASCII $file.FullName
	(Get-Content $file.FullName).Replace('__Project_Name__',   "$ProjectName")   | Set-Content -Encoding ASCII $file.FullName
}

Function IsValidInteger([string]$fnInteger) {
  Try
  {
    $Null = [convert]::ToInt32($fnInteger)
    Return $true
  }
  Catch { Return $false }
}

function AskYesNo {
	param(
	  [Parameter(Mandatory)]
	  [string]$question,
	  [ValidateNotNullOrEmpty()]
	  [string]$default = ""
	)
	
	Do
    {
      $response = Read-Host -Prompt "$question (Y/n)"
      if (! $response) { $response=$default }
      $response = $response.ToLower()
    } Until (($response.ToLower() -eq "y") -or ($response.ToLower() -eq "n"))
	
	if ($response -eq "y") {
		return $true
	}
	return $false
}

function AskOptions {
	param(
	  [Parameter(Mandatory)]
	  [string]$question,
	  [Parameter(Mandatory)]
	  [string[]]$options,
	  [ValidateNotNullOrEmpty()]
	  [int]$default = ""
	)
	
	$count = $options.count
	$size = $count.tostring().length

	Do
	{
	  Write-Host $question
		
	  [int]$i = 1
	  $options | ForEach-Object {
		if ($i -eq $default) {
		  Write-Host (" > {0,-$size}: $PSItem" -f $i)
		}
		else {
		  Write-Host ("   {0,-$size}: $PSItem" -f $i)
		}
		$i = $i + 1
	  }
	
	  Write-Host ""
	  $response = Read-Host -Prompt "[1-$count]"
	  if (! $response) { $response = $default }
	  if (! (IsValidInteger($response))) { $response = $null }
	  [int]$i = [convert]::ToInt32($response)
	  Write-Host "Response : '$response'"
	} Until(($i -ge 1) -and ($i -le $count))
	
	return $i
}

function MarkForRemoval {
	$null > TemplateScripts/marker
}

function AbortSetup {
  if (! (AskYesNo -question "Should this tool run again next time ?" -default "y"))
  {
    MarkForRemoval
  }
  Write-Host "Exiting. No files were changed."
  exit
}

#AskOptions -question "What do you want to do ?" -options "Change values (restart setup)", "Abort setup" -default 2
#exit



#Prompt the user if he wants the tool's assistance
Write-Host "First project setup detected."
if (! (AskYesNo -question "Do you want to run the token replacement tool ?" -default "y"))
{
  AbortSetup
}

# Prompt for the project and workspace names
Do {
  # The actual prompt
  $ProjectName = Read-Host -Prompt "What is the name of the project ? "
  $WorkspaceName = Read-Host -Prompt "What is the name of the project's workspace ? (default: $ProjectName)"
  Write-Host ""
  
  # Cleanup user input
  $ProjectName=$ProjectName.Trim()
  $WorkspaceName=$WorkspaceName.Trim()
  
  # set workspace name to project name if empty
  if (! $WorkspaceName) {
  	$WorkspaceName=$ProjectName
  }
  
  # Give recap to the user
  Write-Host "Current values are"
  Write-Host -NoNewline "  Project name: "
  Write-Host -ForegroundColor Green "$ProjectName"
  Write-Host -NoNewline "  Workspace name: "
  Write-Host -ForegroundColor Green "$WorkspaceName"
  Write-Host "These will be injected in the template."
  Write-Host ""

  #Ensures the user agrees with the new names
  if (! (AskYesNo -question "Are the values Ok ?" -default "y"))
  {
    switch(AskOptions -question "What do you want to do ?" -options "Keep these values and proceed", "Change values (restart setup)", "Abort setup" -default 3)
	{
	  1 { $retry=$false }
      2 { $retry=$true }
      3 { AbortSetup }
    }
  }
} Until($retry -ne $true)


Get-ChildItem -Path .\ -Directory -Recurse |
ForEach-Object {
	RenameEntry($_)
}

Get-ChildItem -Path .\ -Exclude premake,TemplateScripts | Get-ChildItem -File -Recurse |
ForEach-Object {
	RenameEntry($_)
}

Get-ChildItem -Path .\ -Exclude premake,TemplateScripts | Get-ChildItem -File -Recurse |
ForEach-Object {
	ReplaceTokens($_)
}

MarkForRemoval
















