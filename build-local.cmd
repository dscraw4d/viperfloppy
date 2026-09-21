@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo   Viper Universal Windows 10 Floppy Driver - MSVC Build
echo ============================================================
echo.

where cl.exe >nul 2>nul
if errorlevel 1 (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "%VSWHERE%" (
        echo ERROR: Visual Studio Build Tools were not found.
        echo Install Visual Studio 2022 Build Tools with the C++ build tools workload.
        exit /b 1
    )

    for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"

    if not defined VSINSTALL (
        echo ERROR: The MSVC x64 C++ toolchain was not found.
        exit /b 1
    )

    call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat"
    if errorlevel 1 exit /b %errorlevel%
)

if not exist build mkdir build
if not exist bin mkdir bin

echo Building x64 EXE with Microsoft C/C++...
cl.exe /nologo /W4 /O2 /MT /DUNICODE /D_UNICODE /TC ^
  /Fo:"build\ViperUniversalFloppyDriver.obj" ^
  /Fe:"bin\Viper-Universal-Windows-10-Floppy-Driver.exe" ^
  "src\ViperUniversalFloppyDriver.c" ^
  /link /SUBSYSTEM:WINDOWS /MACHINE:X64 setupapi.lib shell32.lib user32.lib kernel32.lib

if errorlevel 1 (
    echo.
    echo BUILD FAILED.
    exit /b %errorlevel%
)

echo.
echo Build complete:
echo   bin\Viper-Universal-Windows-10-Floppy-Driver.exe
echo.
certutil -hashfile "bin\Viper-Universal-Windows-10-Floppy-Driver.exe" SHA256
exit /b 0
