@echo off
setlocal
cd /d "%~dp0"

where clang-cl >nul 2>nul || (
  echo ERROR: clang-cl was not found. Run this from an x64 Visual Studio Developer Command Prompt with LLVM installed.
  exit /b 1
)
where lld-link >nul 2>nul || (
  echo ERROR: lld-link was not found. Install the LLVM tools for Visual Studio.
  exit /b 1
)

if not exist build mkdir build
if not exist bin mkdir bin

clang-cl --target=x86_64-pc-windows-msvc /c src\ViperUniversalFloppyDriver.c /Fobuild\ViperUniversalFloppyDriver.obj /O1 /GS- /GR- /Zl /clang:-ffreestanding /clang:-fno-builtin /clang:-mno-stack-arg-probe
if errorlevel 1 exit /b %errorlevel%

lld-link /out:bin\Viper-Universal-Windows-10-Floppy-Driver.exe /entry:wWinMainCRTStartup /subsystem:windows /nodefaultlib /machine:x64 build\ViperUniversalFloppyDriver.obj kernel32.lib setupapi.lib shell32.lib user32.lib msvcrt.lib
if errorlevel 1 exit /b %errorlevel%

certutil -hashfile bin\Viper-Universal-Windows-10-Floppy-Driver.exe SHA256

echo.
echo Build complete: bin\Viper-Universal-Windows-10-Floppy-Driver.exe
endlocal
