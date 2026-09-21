// Viper Universal Windows 10 Floppy Driver v1.0.2
// Copyright (c) 2026 Darren "Viper" Crawford.
// One-click helper for USB floppy drives on Windows 10 x64.
// Uses signed Microsoft inbox USB floppy drivers from %WINDIR%\INF\usbstor.inf.
// No third-party kernel driver is bundled.

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <setupapi.h>
#include <wchar.h>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")

#ifndef DI_FLAGSEX_ALLOWEXCLUDEDDRVS
#define DI_FLAGSEX_ALLOWEXCLUDEDDRVS 0x00000800L
#endif

#ifndef DI_ENUMSINGLEINF
#define DI_ENUMSINGLEINF 0x00010000L
#endif

#ifndef DI_QUIETINSTALL
#define DI_QUIETINSTALL 0x00800000L
#endif

#define DRIVER_LIST_TYPE SPDIT_CLASSDRIVER
#define ERROR_NOT_FOUND_LOCAL 1168L

// DiInstallDevice is loaded dynamically from newdev.dll so the utility does not
// need to import newdev.lib at link time.
typedef BOOL (WINAPI *PFN_DIINSTALLDEVICE)(
    HWND hwndParent,
    HDEVINFO DeviceInfoSet,
    PSP_DEVINFO_DATA DeviceInfoData,
    PSP_DRVINFO_DATA_W DriverInfoData,
    DWORD Flags,
    PBOOL NeedReboot
);

static WCHAR g_instanceId[1024];
static WCHAR g_prop[1024];
static WCHAR g_windowsDir[MAX_PATH];
static WCHAR g_infPath[MAX_PATH];
static WCHAR g_exePath[MAX_PATH];
static SP_DRVINFO_DATA_W g_bestDriver;

static BOOL contains_i(const WCHAR *haystack, const WCHAR *needle)
{
    size_t hayLen, needleLen, i;

    if (!haystack || !needle || !needle[0]) {
        return FALSE;
    }

    hayLen = wcslen(haystack);
    needleLen = wcslen(needle);
    if (needleLen > hayLen) {
        return FALSE;
    }

    for (i = 0; i + needleLen <= hayLen; ++i) {
        if (CompareStringOrdinal(haystack + i, (int)needleLen,
                                 needle, (int)needleLen, TRUE) == CSTR_EQUAL) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL is_elevated_invocation(void)
{
    return contains_i(GetCommandLineW(), L"/elevated");
}

static void relaunch_elevated(void)
{
    HINSTANCE result;

    if (!GetModuleFileNameW(NULL, g_exePath, ARRAYSIZE(g_exePath))) {
        MessageBoxW(NULL,
                    L"Could not determine the installer path.",
                    L"Viper Universal Windows 10 Floppy Driver",
                    MB_OK | MB_ICONERROR);
        return;
    }

    result = ShellExecuteW(NULL, L"runas", g_exePath, L"/elevated", NULL, SW_SHOWNORMAL);
    if ((INT_PTR)result <= 32) {
        MessageBoxW(NULL,
                    L"Administrator permission is required to install a device driver.",
                    L"Viper Universal Windows 10 Floppy Driver",
                    MB_OK | MB_ICONWARNING);
    }
}

static BOOL get_device_property(HDEVINFO deviceSet,
                                PSP_DEVINFO_DATA deviceData,
                                DWORD property,
                                WCHAR *output,
                                DWORD outputChars)
{
    DWORD regType = 0;
    DWORD requiredBytes = 0;

    if (!output || outputChars == 0) {
        return FALSE;
    }

    output[0] = L'\0';
    return SetupDiGetDeviceRegistryPropertyW(deviceSet,
                                             deviceData,
                                             property,
                                             &regType,
                                             (PBYTE)output,
                                             outputChars * sizeof(WCHAR),
                                             &requiredBytes);
}

static BOOL target_name_match(const WCHAR *text)
{
    return text && (contains_i(text, L"SFD-321") ||
                    contains_i(text, L"Floppy") ||
                    contains_i(text, L"USB FDD"));
}

static BOOL find_target_device(WCHAR *instanceOut, DWORD instanceChars)
{
    HDEVINFO devices;
    DWORD index = 0;
    SP_DEVINFO_DATA data;

    devices = SetupDiGetClassDevsW(NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (devices == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    for (;;) {
        BOOL isUsb = FALSE;
        BOOL match = FALSE;

        ZeroMemory(&data, sizeof(data));
        data.cbSize = sizeof(data);

        if (!SetupDiEnumDeviceInfo(devices, index++, &data)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS) {
                break;
            }
            continue;
        }

        g_instanceId[0] = L'\0';
        if (!SetupDiGetDeviceInstanceIdW(devices, &data, g_instanceId,
                                         ARRAYSIZE(g_instanceId), NULL)) {
            continue;
        }

        isUsb = contains_i(g_instanceId, L"USB\\");
        match = contains_i(g_instanceId, L"VID_055D&PID_2020");

        if (!match && isUsb) {
            if (get_device_property(devices, &data, SPDRP_FRIENDLYNAME,
                                    g_prop, ARRAYSIZE(g_prop)) && target_name_match(g_prop)) {
                match = TRUE;
            }
        }

        if (!match && isUsb) {
            if (get_device_property(devices, &data, SPDRP_DEVICEDESC,
                                    g_prop, ARRAYSIZE(g_prop)) && target_name_match(g_prop)) {
                match = TRUE;
            }
        }

        if (match) {
            if (wcsncpy_s(instanceOut, instanceChars, g_instanceId, _TRUNCATE) == 0) {
                SetupDiDestroyDeviceInfoList(devices);
                return TRUE;
            }
        }
    }

    SetupDiDestroyDeviceInfoList(devices);
    return FALSE;
}

static int driver_score(const SP_DRVINFO_DATA_W *driver)
{
    if (contains_i(driver->Description, L"NEC USB Floppy")) return 100;
    if (contains_i(driver->MfgName, L"NEC") && contains_i(driver->Description, L"Floppy")) return 95;
    if (contains_i(driver->Description, L"TEAC USB Floppy")) return 90;
    if (contains_i(driver->MfgName, L"TEAC") && contains_i(driver->Description, L"Floppy")) return 85;
    if (contains_i(driver->Description, L"USB Floppy")) return 70;
    return 0;
}

static DWORD install_inbox_floppy_driver(const WCHAR *instanceId, BOOL *rebootNeeded)
{
    HDEVINFO set = INVALID_HANDLE_VALUE;
    SP_DEVINFO_DATA devData;
    SP_DEVINSTALL_PARAMS_W params;
    SP_DRVINFO_DATA_W drv;
    DWORD idx = 0;
    int bestScore = 0;
    BOOL listBuilt = FALSE;
    HMODULE newdev = NULL;
    PFN_DIINSTALLDEVICE diInstallDevice = NULL;
    BOOL needReboot = FALSE;
    DWORD err = ERROR_SUCCESS;

    *rebootNeeded = FALSE;

    if (!GetWindowsDirectoryW(g_windowsDir, ARRAYSIZE(g_windowsDir))) {
        return GetLastError();
    }

    if (swprintf_s(g_infPath, ARRAYSIZE(g_infPath), L"%s\\INF\\usbstor.inf", g_windowsDir) < 0) {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    set = SetupDiCreateDeviceInfoList(NULL, NULL);
    if (set == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    ZeroMemory(&devData, sizeof(devData));
    devData.cbSize = sizeof(devData);

    if (!SetupDiOpenDeviceInfoW(set, instanceId, NULL, 0, &devData)) {
        err = GetLastError();
        goto cleanup;
    }

    if (!SetupDiSetSelectedDevice(set, &devData)) {
        err = GetLastError();
        goto cleanup;
    }

    ZeroMemory(&params, sizeof(params));
    params.cbSize = sizeof(params);

    if (!SetupDiGetDeviceInstallParamsW(set, &devData, &params)) {
        err = GetLastError();
        goto cleanup;
    }

    params.Flags |= (DI_ENUMSINGLEINF | DI_QUIETINSTALL);
    params.FlagsEx |= DI_FLAGSEX_ALLOWEXCLUDEDDRVS;

    if (wcsncpy_s(params.DriverPath, ARRAYSIZE(params.DriverPath), g_infPath, _TRUNCATE) != 0) {
        err = ERROR_INSUFFICIENT_BUFFER;
        goto cleanup;
    }

    if (!SetupDiSetDeviceInstallParamsW(set, &devData, &params)) {
        err = GetLastError();
        goto cleanup;
    }

    if (!SetupDiBuildDriverInfoList(set, &devData, DRIVER_LIST_TYPE)) {
        err = GetLastError();
        goto cleanup;
    }
    listBuilt = TRUE;

    ZeroMemory(&g_bestDriver, sizeof(g_bestDriver));

    for (;;) {
        int score;

        ZeroMemory(&drv, sizeof(drv));
        drv.cbSize = sizeof(drv);

        if (!SetupDiEnumDriverInfoW(set, &devData, DRIVER_LIST_TYPE, idx++, &drv)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS) {
                break;
            }
            continue;
        }

        score = driver_score(&drv);
        if (score > bestScore) {
            bestScore = score;
            g_bestDriver = drv;
        }
    }

    if (bestScore == 0) {
        err = ERROR_NOT_FOUND_LOCAL;
        goto cleanup;
    }

    if (!SetupDiSetSelectedDriverW(set, &devData, &g_bestDriver)) {
        err = GetLastError();
        goto cleanup;
    }

    newdev = LoadLibraryW(L"newdev.dll");
    if (!newdev) {
        err = GetLastError();
        goto cleanup;
    }

    diInstallDevice = (PFN_DIINSTALLDEVICE)GetProcAddress(newdev, "DiInstallDevice");
    if (!diInstallDevice) {
        err = GetLastError();
        goto cleanup;
    }

    if (!diInstallDevice(NULL, set, &devData, &g_bestDriver, 0, &needReboot)) {
        err = GetLastError();
        goto cleanup;
    }

    *rebootNeeded = needReboot;
    err = ERROR_SUCCESS;

cleanup:
    if (newdev) {
        FreeLibrary(newdev);
    }
    if (listBuilt) {
        SetupDiDestroyDriverInfoList(set, &devData, DRIVER_LIST_TYPE);
    }
    if (set != INVALID_HANDLE_VALUE) {
        SetupDiDestroyDeviceInfoList(set);
    }
    return err;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR commandLine, int showCommand)
{
    WCHAR target[1024];
    BOOL reboot = FALSE;
    DWORD err;

    UNREFERENCED_PARAMETER(instance);
    UNREFERENCED_PARAMETER(previousInstance);
    UNREFERENCED_PARAMETER(commandLine);
    UNREFERENCED_PARAMETER(showCommand);

    if (!is_elevated_invocation()) {
        relaunch_elevated();
        return 0;
    }

    MessageBoxW(NULL,
        L"Connect the USB floppy drive now, then click OK.\n\n"
        L"This installer uses the signed Microsoft USB floppy driver already included with Windows 10.",
        L"Viper Universal Windows 10 Floppy Driver", MB_OK | MB_ICONINFORMATION);

    target[0] = L'\0';
    if (!find_target_device(target, ARRAYSIZE(target))) {
        MessageBoxW(NULL,
            L"No supported USB floppy drive was found.\n\n"
            L"Make sure the drive is plugged directly into the PC and appears in Device Manager.\n"
            L"The installer recognizes USB devices whose name contains Floppy, USB FDD, or SFD-321, plus the known Samsung VID_055D / PID_2020 device.",
            L"Viper Universal Windows 10 Floppy Driver", MB_OK | MB_ICONWARNING);
        return 2;
    }

    err = install_inbox_floppy_driver(target, &reboot);
    if (err == ERROR_SUCCESS) {
        if (reboot) {
            MessageBoxW(NULL,
                L"Driver installed successfully.\n\nWindows reports that a restart is required. After rebooting, the floppy drive should appear as A: (or another drive letter).",
                L"Viper Floppy Driver Installed", MB_OK | MB_ICONINFORMATION);
        } else {
            MessageBoxW(NULL,
                L"Driver installed successfully.\n\nUnplug the floppy drive, plug it back in, and check File Explorer. It should appear as A: (or another drive letter).",
                L"Viper Floppy Driver Installed", MB_OK | MB_ICONINFORMATION);
        }
        return 0;
    }

    if (err == ERROR_NOT_FOUND_LOCAL) {
        MessageBoxW(NULL,
            L"A USB floppy drive was detected, but this Windows 10 installation does not expose a compatible NEC/TEAC USB floppy entry in usbstor.inf.\n\n"
            L"No third-party or unsigned driver was installed. Try Windows Update, or use Device Manager -> Update driver -> Let me pick -> Show All Devices -> NEC Systems -> NEC USB Floppy.",
            L"Compatible Inbox Driver Not Found", MB_OK | MB_ICONWARNING);
        return 3;
    }

    MessageBoxW(NULL,
        L"Windows detected the USB floppy drive, but the driver installation did not complete.\n\n"
        L"No unsigned driver was installed. Open Device Manager and verify the drive is still connected, then run this installer again.",
        L"Viper Floppy Driver Install Error", MB_OK | MB_ICONERROR);
    return (int)err;
}
