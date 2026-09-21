# Viper Universal Windows 10 Floppy Driver v1.0.0

**Created by Darren "Viper" Crawford**

Initial public release of Viper Universal Windows 10 Floppy Driver.

## Features

- One-click Windows 10 x64 USB floppy driver repair/install helper.
- Automatic Administrator/UAC elevation.
- Detects USB devices identified as floppy drives or USB FDD devices.
- Explicit Samsung SFD-321 family detection.
- Explicit recognition of the known Samsung `USB\\VID_055D&PID_2020` hardware ID.
- Uses Windows' signed `usbstor.inf` inbox drivers rather than bundling an unsigned replacement driver.
- Prefers NEC USB Floppy and TEAC USB Floppy driver entries.
- Safe failure behavior: if Windows does not expose a compatible inbox driver, the program makes no unsigned-driver substitution.

## Recommended GitHub release

**Tag:** `v1.0.0`

**Release title:** `Viper Universal Windows 10 Floppy Driver v1.0.0`

Attach:

- `Viper-Universal-Windows-10-Floppy-Driver.exe`
- `SHA256SUMS.txt`

## Notes

The included EXE is not Authenticode-signed, so Windows SmartScreen may warn that the publisher is unknown. Users should verify the supplied SHA-256 checksum before running it.
