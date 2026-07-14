@echo off
setlocal

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo Visual Studio Build Tools not found.
    exit /b 1
)

for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS=%%I"
if not defined VS (
    echo MASM x86 tools not found.
    exit /b 1
)

call "%VS%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
where Crinkler.exe >nul 2>nul
if errorlevel 1 (
    echo Crinkler.exe is not on PATH.
    exit /b 1
)

ml /nologo /c /coff /Fosnip.obj src\main.asm
if errorlevel 1 exit /b 1

del /q snip.exe 2>nul
if exist snip.exe (
    echo snip.exe is running or locked. Close it and rebuild.
    exit /b 1
)

Crinkler.exe snip.obj kernel32.lib user32.lib gdi32.lib comdlg32.lib gdiplus.lib dwmapi.lib ^
    /LIBPATH:"%WindowsSdkDir%Lib\%WindowsSDKLibVersion%um\x86" ^
    /SUBSYSTEM:WINDOWS /ENTRY:start /OUT:snip.exe ^
    /COMPMODE:INSTANT /REPORT:snip-report.html
if errorlevel 1 exit /b 1

for %%I in (snip.exe) do echo Built snip.exe: %%~zI bytes
