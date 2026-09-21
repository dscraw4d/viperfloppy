# Building Viper Universal Windows 10 Floppy Driver

## Requirements

The included source is deliberately small and does not require the Windows SDK header files, but it does require a Windows x64 linker environment containing the standard Windows import libraries.

Recommended environment:

- Windows 10 or Windows 11 build machine
- Visual Studio 2022 Build Tools / Visual Studio 2022
- LLVM/Clang tools (`clang-cl` and `lld-link`)

Run the build from an **x64 Native Tools Command Prompt for Visual Studio**.

## Build command

```bat
clang-cl --target=x86_64-pc-windows-msvc /c src\ViperUniversalFloppyDriver.c /Fobuild\ViperUniversalFloppyDriver.obj /O1 /GS- /GR- /Zl /clang:-ffreestanding /clang:-fno-builtin /clang:-mno-stack-arg-probe

lld-link /out:bin\Viper-Universal-Windows-10-Floppy-Driver.exe /entry:wWinMainCRTStartup /subsystem:windows /nodefaultlib /machine:x64 build\ViperUniversalFloppyDriver.obj kernel32.lib setupapi.lib shell32.lib user32.lib msvcrt.lib
```

Or run `build-local.cmd`.

## GitHub Actions

The repository contains `.github/workflows/build.yml`.

GitHub Actions will:

1. Check out the repository.
2. initialize the Visual Studio x64 toolchain.
3. compile the x64 EXE with `clang-cl` and `lld-link`.
4. calculate a SHA-256 checksum.
5. upload the executable and checksum as a workflow artifact.

The workflow uses the current Node 24-generation GitHub Actions majors rather than the old Node 20 actions.
