@echo off
chcp 65001 > nul

:: ============== Build Settings ==============
:: Action: Build / Clean / Rebuild (Accepts %1 argument if provided)
if "%~1"=="" (set ACTION=Rebuild) else (set ACTION=%~1)
:: Configuration: Release / Debug
set CONFIG=Debug
:: Platform: Win32 / Win64
set TARGET_PLATFORM=Win64
:: ============================================

:: Print build details
echo Launching MSBuild process...
echo Action:        %ACTION%
echo Configuration: %CONFIG%
echo Platform:      %TARGET_PLATFORM%
echo ------------------------------------------

:: Load C++ Builder environment variables (BDS, Include/Lib folders, etc.)
call "c:\Program Files (x86)\Embarcadero\Studio\21.0\bin\rsvars.bat"

:: Path to MSBuild 2022 executable
set "MSBUILD32_PATH=c:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
set "MSBUILD64_PATH=c:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe"

if "%TARGET_PLATFORM%"=="Win64" (
    set "MSBUILD_PATH=%MSBUILD64_PATH%"
) else (
    set "MSBUILD_PATH=%MSBUILD32_PATH%"
)

echo Using MSBuild from: "%MSBUILD_PATH%"
echo ------------------------------------------

"%MSBUILD_PATH%" rpg.cbproj /t:%ACTION% /p:Config=%CONFIG% /p:Platform=%TARGET_PLATFORM%

pause
