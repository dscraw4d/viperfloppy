@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo   Viper Universal Windows 10 Floppy Driver - MSVC Build
echo ============================================================
echo.

rem If cl.exe is already available (for example after vcvars64.bat), use it.
where cl.exe >nul 2>nul
if not errorlevel 1 goto :build

rem Visual Studio 2026 (18.x) - GitHub hosted runner and local editions.
set "VCVARS64="
for %%P in (
  "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
  "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
) do (
  if not defined VCVARS64 if exist "%%~P" set "VCVARS64=%%~P"
)

if not defined VCVARS64 (
  echo ERROR: Microsoft C/C++ build environment was not found.
  echo.
  echo Supported local build environments:
  echo   - Visual Studio 2026 with Desktop development with C++
  echo   - Visual Studio 2022 with Desktop development with C++
  echo   - Visual Studio 2022 Build Tools with C++ build tools
  echo.
  echo GitHub Actions does not require you to install these locally.
  exit /b 1
)

echo Initializing MSVC environment:
echo   %VCVARS64%
call "%VCVARS64%"
if errorlevel 1 (
  echo ERROR: vcvars64.bat failed.
  exit /b %errorlevel%
)

where cl.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: vcvars64.bat completed but cl.exe is still unavailable.
  exit /b 1
)

:build
if not exist build mkdir build
if not exist bin mkdir bin

echo Compiler:
where cl.exe
cl.exe 2>&1 | findstr /C:"Version" >nul 2>nul

echo.
echo Building x64 Windows 10 EXE with Microsoft C/C++...
cl.exe /nologo /W4 /O2 /MT /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /TC ^
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
