# Viper Universal Windows 10 Floppy Driver v1.0.2 Hotfix

This hotfix fixes GitHub Actions builds that failed with:

    ERROR: Visual Studio Build Tools were not found.

## Cause

GitHub's current Windows Server 2025 + Visual Studio 2026 runner installs Visual Studio Enterprise 2026 under:

    C:\Program Files\Microsoft Visual Studio\18\Enterprise

The v1.0.1 local build detector expected the older Visual Studio installer/vswhere layout and could fail when GitHub routed the job to the VS2026 image.

## Fix

v1.0.2:

- Pins GitHub Actions to `windows-2025-vs2026`.
- Initializes the official VS2026 x64 developer environment directly with `vcvars64.bat`.
- Verifies `cl.exe` and `link.exe` before compiling.
- Keeps `build-local.cmd` compatible with VS2026 and VS2022 installations.
- Defines `_WIN32_WINNT=0x0A00` to make the Windows 10 target explicit.
- Retains static CRT linking (`/MT`).

The floppy-driver installation logic itself is unchanged from v1.0.1.
