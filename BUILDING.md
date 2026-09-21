# Building Viper Universal Windows 10 Floppy Driver

## v1.0.1 build system

The project now uses Microsoft's own MSVC compiler and linker. The previous v1.0.0 GitHub workflow used `clang-cl` + `lld-link` in freestanding `/nodefaultlib` mode and could fail on GitHub Actions with:

```text
lld-link: error: undefined symbol: memcpy
```

v1.0.1 removes that brittle LLVM/freestanding build path.

## Requirements for a local build

- Windows 10 or Windows 11
- Visual Studio 2022 Build Tools (or full Visual Studio)
- **Desktop development with C++** / MSVC x64 toolchain
- Windows 10/11 SDK

## Build locally

From a normal Command Prompt, run:

```bat
build-local.cmd
```

The script locates Visual Studio with `vswhere.exe`, initializes the x64 developer environment, and builds:

```text
bin\Viper-Universal-Windows-10-Floppy-Driver.exe
```

The EXE is built with `/MT`, so the Microsoft C runtime is linked statically and users do not need to install a separate Visual C++ runtime package for this utility.

## GitHub Actions

`.github/workflows/build.yml` is pinned to `windows-2022` and invokes the same `build-local.cmd` script. This intentionally avoids `windows-latest` toolchain churn and does not require LLVM, `clang-cl`, or `lld-link`.
