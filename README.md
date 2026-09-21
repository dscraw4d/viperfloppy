# Viper Universal Windows 10 Floppy Driver

A one-click Windows 10 x64 helper for USB floppy drives that have stopped working correctly or are not automatically bound to Windows' built-in USB floppy driver.

**Created by Darren "Viper" Crawford**

## What it does

- Detects attached USB floppy/FDD devices.
- Includes explicit support for Samsung-family **SFD-321** devices, including the known `USB\\VID_055D&PID_2020` ID.
- Uses Microsoft's signed Windows 10 inbox driver package at `%WINDIR%\\INF\\usbstor.inf`.
- Prefers **NEC USB Floppy**, then **TEAC USB Floppy**, then another matching USB floppy entry exposed by Windows.
- Uses Windows SetupAPI/NewDev to bind the selected inbox driver to the detected USB floppy drive.
- Requests Administrator permission through UAC automatically.
- Does **not** install an unsigned third-party kernel driver.

## Download / run

The compiled program is in:

`bin/Viper-Universal-Windows-10-Floppy-Driver.exe`

1. Plug the USB floppy drive directly into the PC.
2. Run `Viper-Universal-Windows-10-Floppy-Driver.exe`.
3. Accept the Windows UAC prompt.
4. Click **OK** when the program asks you to confirm the drive is connected.
5. If installation succeeds, unplug and reconnect the USB floppy drive.
6. Check File Explorer for the floppy drive, commonly `A:`.

## Windows SmartScreen

The supplied EXE is not Authenticode-signed. Windows SmartScreen may therefore show an **Unrecognized app** warning. Verify the SHA-256 hash in `SHA256SUMS.txt` before running it.

## Supported systems

- Windows 10 x64
- USB floppy drives exposed by Windows as a USB floppy/FDD device
- Samsung SFD-321 family explicitly recognized

This utility is intentionally conservative. It will not force a floppy driver onto arbitrary USB mass-storage devices.

## If the drive is not detected

Open:

**Device Manager -> your floppy drive -> Properties -> Details -> Hardware Ids**

Record the `USB\\VID_xxxx&PID_xxxx` value. Additional hardware IDs can be added to the detector in `src/ViperUniversalFloppyDriver.c`.

## How it works

This project is an installer/helper, not a replacement kernel driver. It searches Windows' own `usbstor.inf` for a compatible USB floppy driver and asks Windows to install that signed inbox driver for the selected device.

That approach avoids distributing old Windows 98/2000-era `.SYS` files and avoids normal Windows 10 x64 unsigned-driver restrictions.

## Source and building

See [BUILDING.md](BUILDING.md).

## Release

Current package: **v1.0.0**

See [RELEASE_NOTES.md](RELEASE_NOTES.md).

## Disclaimer

Use at your own risk. Back up important floppy disks before performing write tests. This project is not affiliated with Microsoft, Samsung, NEC, or TEAC.
