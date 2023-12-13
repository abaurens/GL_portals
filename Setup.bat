@if exist .\TemplateScripts\RenameTemplate.ps1 (
	@Powershell.exe -executionpolicy remotesigned -File .\TemplateScripts\RenameTemplate.ps1
	@if exist .\TemplateScripts\Marker (
	  @RD /S /Q ".\TemplateScripts"

	  git add --all
	  git commit -m "Initial project setup"
	  git push origin master
	)
)


call premake\Windows\premake5.exe vs2022 || (
	pause
)
