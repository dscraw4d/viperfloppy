// Viper Universal Windows 10 Floppy Driver v1.0
// Copyright (c) 2026 Darren "Viper" Crawford.
// One-click helper for USB floppy drives on Windows 10 x64.
// Uses signed Microsoft inbox USB floppy drivers from %WINDIR%\INF\usbstor.inf.
// No third-party kernel driver is bundled.

#define UNICODE 1
#define _UNICODE 1

typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long long ULONGLONG;
typedef unsigned long long ULONG_PTR;
typedef void* HANDLE;
typedef void* HWND;
typedef void* HINSTANCE;
typedef void* HMODULE;
typedef void* HDEVINFO;
typedef void* HSPFILEQ;
typedef void* PVOID;
typedef const void* LPCVOID;
typedef unsigned short WCHAR;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;
typedef void* FARPROC;
typedef unsigned long ULONG;
typedef long LONG;

#define WINAPI __attribute__((ms_abi))
#define CALLBACK __attribute__((ms_abi))
#define TRUE 1
#define FALSE 0
#define NULLPTR ((void*)0)
#define INVALID_HANDLE_VALUE ((void*)(long long)-1)
#define MAX_PATH 260
#define LINE_LEN 256

#define MB_OK 0x00000000UL
#define MB_ICONINFORMATION 0x00000040UL
#define MB_ICONWARNING 0x00000030UL
#define MB_ICONERROR 0x00000010UL
#define SW_SHOWNORMAL 1

#define DIGCF_PRESENT 0x00000002UL
#define DIGCF_ALLCLASSES 0x00000004UL
#define SPDRP_DEVICEDESC 0x00000000UL
#define SPDRP_FRIENDLYNAME 0x0000000CUL
#define SPDIT_CLASSDRIVER 0x00000001UL
#define DI_ENUMSINGLEINF 0x00010000UL
#define DI_QUIETINSTALL 0x00800000UL
#define DI_FLAGSEX_ALLOWEXCLUDEDDRVS 0x00000800UL
#define DI_NEEDRESTART 0x00000080UL
#define DI_NEEDREBOOT 0x00000100UL
#define ERROR_NO_MORE_ITEMS 259UL

struct GUID_ {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[8];
};
typedef struct GUID_ GUID;

typedef struct _SP_DEVINFO_DATA {
    DWORD cbSize;
    GUID ClassGuid;
    DWORD DevInst;
    ULONG_PTR Reserved;
} SP_DEVINFO_DATA;

typedef unsigned int (CALLBACK *PSP_FILE_CALLBACK_W)(PVOID, unsigned int, ULONG_PTR, ULONG_PTR);

typedef struct _SP_DEVINSTALL_PARAMS_W {
    DWORD cbSize;
    DWORD Flags;
    DWORD FlagsEx;
    HWND hwndParent;
    PSP_FILE_CALLBACK_W InstallMsgHandler;
    PVOID InstallMsgHandlerContext;
    HSPFILEQ FileQueue;
    ULONG_PTR ClassInstallReserved;
    DWORD Reserved;
    WCHAR DriverPath[MAX_PATH];
} SP_DEVINSTALL_PARAMS_W;

typedef struct _FILETIME_ {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME_;

typedef struct _SP_DRVINFO_DATA_W {
    DWORD cbSize;
    DWORD DriverType;
    ULONG_PTR Reserved;
    WCHAR Description[LINE_LEN];
    WCHAR MfgName[LINE_LEN];
    WCHAR ProviderName[LINE_LEN];
    FILETIME_ DriverDate;
    ULONGLONG DriverVersion;
} SP_DRVINFO_DATA_W;

// kernel32
__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE, LPWSTR, DWORD);
__declspec(dllimport) LPWSTR WINAPI GetCommandLineW(void);
__declspec(dllimport) DWORD WINAPI GetWindowsDirectoryW(LPWSTR, DWORD);
__declspec(dllimport) DWORD WINAPI GetLastError(void);
__declspec(dllimport) HMODULE WINAPI LoadLibraryW(LPCWSTR);
__declspec(dllimport) FARPROC WINAPI GetProcAddress(HMODULE, const char*);
__declspec(dllimport) BOOL WINAPI FreeLibrary(HMODULE);

// shell32 / user32
__declspec(dllimport) HINSTANCE WINAPI ShellExecuteW(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, int);
__declspec(dllimport) int WINAPI MessageBoxW(HWND, LPCWSTR, LPCWSTR, unsigned int);

// setupapi
__declspec(dllimport) HDEVINFO WINAPI SetupDiGetClassDevsW(const GUID*, LPCWSTR, HWND, DWORD);
__declspec(dllimport) BOOL WINAPI SetupDiEnumDeviceInfo(HDEVINFO, DWORD, SP_DEVINFO_DATA*);
__declspec(dllimport) BOOL WINAPI SetupDiGetDeviceInstanceIdW(HDEVINFO, SP_DEVINFO_DATA*, LPWSTR, DWORD, DWORD*);
__declspec(dllimport) BOOL WINAPI SetupDiGetDeviceRegistryPropertyW(HDEVINFO, SP_DEVINFO_DATA*, DWORD, DWORD*, BYTE*, DWORD, DWORD*);
__declspec(dllimport) BOOL WINAPI SetupDiDestroyDeviceInfoList(HDEVINFO);
__declspec(dllimport) HDEVINFO WINAPI SetupDiCreateDeviceInfoList(const GUID*, HWND);
__declspec(dllimport) BOOL WINAPI SetupDiOpenDeviceInfoW(HDEVINFO, LPCWSTR, HWND, DWORD, SP_DEVINFO_DATA*);
__declspec(dllimport) BOOL WINAPI SetupDiSetSelectedDevice(HDEVINFO, SP_DEVINFO_DATA*);
__declspec(dllimport) BOOL WINAPI SetupDiGetDeviceInstallParamsW(HDEVINFO, SP_DEVINFO_DATA*, SP_DEVINSTALL_PARAMS_W*);
__declspec(dllimport) BOOL WINAPI SetupDiSetDeviceInstallParamsW(HDEVINFO, SP_DEVINFO_DATA*, SP_DEVINSTALL_PARAMS_W*);
__declspec(dllimport) BOOL WINAPI SetupDiBuildDriverInfoList(HDEVINFO, SP_DEVINFO_DATA*, DWORD);
__declspec(dllimport) BOOL WINAPI SetupDiEnumDriverInfoW(HDEVINFO, SP_DEVINFO_DATA*, DWORD, DWORD, SP_DRVINFO_DATA_W*);
__declspec(dllimport) BOOL WINAPI SetupDiSetSelectedDriverW(HDEVINFO, SP_DEVINFO_DATA*, SP_DRVINFO_DATA_W*);
__declspec(dllimport) BOOL WINAPI SetupDiDestroyDriverInfoList(HDEVINFO, SP_DEVINFO_DATA*, DWORD);

typedef BOOL (WINAPI *PFN_DIINSTALLDEVICE)(HWND, HDEVINFO, SP_DEVINFO_DATA*, SP_DRVINFO_DATA_W*, DWORD, BOOL*);


static void* memcpy(void* dst, const void* src, unsigned long long n) {
    BYTE* d = (BYTE*)dst; const BYTE* s = (const BYTE*)src;
    while (n--) *d++ = *s++;
    return dst;
}

static WCHAR g_instanceId[1024];
static WCHAR g_prop[1024];
static WCHAR g_windowsDir[MAX_PATH];
static WCHAR g_infPath[MAX_PATH];
static WCHAR g_exePath[MAX_PATH];
static SP_DRVINFO_DATA_W g_bestDriver;

static void zero_bytes(void* p, unsigned long long n) {
    volatile BYTE* b = (volatile BYTE*)p;
    while (n--) *b++ = 0;
}

static unsigned int wlen_local(const WCHAR* s) {
    unsigned int n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static WCHAR lower_ascii_w(WCHAR c) {
    if (c >= L'A' && c <= L'Z') return (WCHAR)(c + (L'a' - L'A'));
    return c;
}

static BOOL contains_i(const WCHAR* hay, const WCHAR* needle) {
    unsigned int i, j;
    if (!hay || !needle || !*needle) return FALSE;
    for (i = 0; hay[i]; ++i) {
        for (j = 0; needle[j]; ++j) {
            if (!hay[i+j]) return FALSE;
            if (lower_ascii_w(hay[i+j]) != lower_ascii_w(needle[j])) break;
        }
        if (!needle[j]) return TRUE;
    }
    return FALSE;
}

static void wcopy(WCHAR* dst, unsigned int cap, const WCHAR* src) {
    unsigned int i = 0;
    if (!cap) return;
    while (src && src[i] && i + 1 < cap) { dst[i] = src[i]; ++i; }
    dst[i] = 0;
}

static BOOL wappend(WCHAR* dst, unsigned int cap, const WCHAR* src) {
    unsigned int n = wlen_local(dst), i = 0;
    if (n >= cap) return FALSE;
    while (src && src[i]) {
        if (n + i + 1 >= cap) return FALSE;
        dst[n+i] = src[i];
        ++i;
    }
    dst[n+i] = 0;
    return TRUE;
}

static BOOL is_elevated_invocation(void) {
    LPWSTR cmd = GetCommandLineW();
    return contains_i(cmd, L"/elevated");
}

static void relaunch_elevated(void) {
    if (!GetModuleFileNameW(NULLPTR, g_exePath, MAX_PATH)) {
        MessageBoxW(NULLPTR, L"Could not determine the installer path.", L"Viper Universal Windows 10 Floppy Driver", MB_OK | MB_ICONERROR);
        return;
    }
    HINSTANCE r = ShellExecuteW(NULLPTR, L"runas", g_exePath, L"/elevated", NULLPTR, SW_SHOWNORMAL);
    if ((long long)r <= 32) {
        MessageBoxW(NULLPTR, L"Administrator permission is required to install a device driver.", L"Viper Universal Windows 10 Floppy Driver", MB_OK | MB_ICONWARNING);
    }
}

static BOOL get_prop(HDEVINFO devs, SP_DEVINFO_DATA* data, DWORD prop, WCHAR* out, DWORD outChars) {
    DWORD regType = 0, req = 0;
    if (!outChars) return FALSE;
    out[0] = 0;
    return SetupDiGetDeviceRegistryPropertyW(devs, data, prop, &regType, (BYTE*)out, outChars * sizeof(WCHAR), &req);
}

static BOOL find_target_device(WCHAR* instanceOut, DWORD instanceChars) {
    HDEVINFO devs = SetupDiGetClassDevsW(NULLPTR, NULLPTR, NULLPTR, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    DWORD index = 0;
    SP_DEVINFO_DATA data;
    if (devs == INVALID_HANDLE_VALUE) return FALSE;

    for (;;) {
        zero_bytes(&data, sizeof(data));
        data.cbSize = sizeof(data);
        if (!SetupDiEnumDeviceInfo(devs, index++, &data)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS) break;
            continue;
        }
        g_instanceId[0] = 0;
        SetupDiGetDeviceInstanceIdW(devs, &data, g_instanceId, 1024, NULLPTR);

        BOOL isUsb = contains_i(g_instanceId, L"USB\\");
        BOOL match = contains_i(g_instanceId, L"VID_055D&PID_2020");
        if (!match && isUsb) {
            g_prop[0] = 0;
            if (get_prop(devs, &data, SPDRP_FRIENDLYNAME, g_prop, 1024)) {
                if (contains_i(g_prop, L"SFD-321") || contains_i(g_prop, L"Floppy") || contains_i(g_prop, L"USB FDD")) match = TRUE;
            }
        }
        if (!match && isUsb) {
            g_prop[0] = 0;
            if (get_prop(devs, &data, SPDRP_DEVICEDESC, g_prop, 1024)) {
                if (contains_i(g_prop, L"SFD-321") || contains_i(g_prop, L"Floppy") || contains_i(g_prop, L"USB FDD")) match = TRUE;
            }
        }
        if (match && g_instanceId[0]) {
            wcopy(instanceOut, instanceChars, g_instanceId);
            SetupDiDestroyDeviceInfoList(devs);
            return TRUE;
        }
    }
    SetupDiDestroyDeviceInfoList(devs);
    return FALSE;
}

static int driver_score(const SP_DRVINFO_DATA_W* d) {
    if (contains_i(d->Description, L"NEC USB Floppy")) return 100;
    if (contains_i(d->MfgName, L"NEC") && contains_i(d->Description, L"Floppy")) return 95;
    if (contains_i(d->Description, L"TEAC USB Floppy")) return 90;
    if (contains_i(d->MfgName, L"TEAC") && contains_i(d->Description, L"Floppy")) return 85;
    if (contains_i(d->Description, L"USB Floppy")) return 70;
    return 0;
}

static DWORD install_inbox_floppy_driver(const WCHAR* instanceId, BOOL* rebootNeeded) {
    HDEVINFO set = INVALID_HANDLE_VALUE;
    SP_DEVINFO_DATA devData;
    SP_DEVINSTALL_PARAMS_W params;
    SP_DRVINFO_DATA_W drv;
    DWORD idx, bestScore = 0;
    BOOL listBuilt = FALSE;
    HMODULE newdev = NULLPTR;
    PFN_DIINSTALLDEVICE diInstallDevice = NULLPTR;
    BOOL needReboot = FALSE;
    DWORD err = 0;

    *rebootNeeded = FALSE;
    if (!GetWindowsDirectoryW(g_windowsDir, MAX_PATH)) return GetLastError();
    wcopy(g_infPath, MAX_PATH, g_windowsDir);
    if (!wappend(g_infPath, MAX_PATH, L"\\INF\\usbstor.inf")) return 87;

    set = SetupDiCreateDeviceInfoList(NULLPTR, NULLPTR);
    if (set == INVALID_HANDLE_VALUE) return GetLastError();

    zero_bytes(&devData, sizeof(devData));
    devData.cbSize = sizeof(devData);
    if (!SetupDiOpenDeviceInfoW(set, instanceId, NULLPTR, 0, &devData)) { err = GetLastError(); goto cleanup; }
    if (!SetupDiSetSelectedDevice(set, &devData)) { err = GetLastError(); goto cleanup; }

    zero_bytes(&params, sizeof(params));
    params.cbSize = sizeof(params);
    if (!SetupDiGetDeviceInstallParamsW(set, &devData, &params)) { err = GetLastError(); goto cleanup; }
    params.Flags |= (DI_ENUMSINGLEINF | DI_QUIETINSTALL);
    params.FlagsEx |= DI_FLAGSEX_ALLOWEXCLUDEDDRVS;
    wcopy(params.DriverPath, MAX_PATH, g_infPath);
    if (!SetupDiSetDeviceInstallParamsW(set, &devData, &params)) { err = GetLastError(); goto cleanup; }

    if (!SetupDiBuildDriverInfoList(set, &devData, SPDIT_CLASSDRIVER)) { err = GetLastError(); goto cleanup; }
    listBuilt = TRUE;

    zero_bytes(&g_bestDriver, sizeof(g_bestDriver));
    idx = 0;
    for (;;) {
        zero_bytes(&drv, sizeof(drv));
        drv.cbSize = sizeof(drv);
        if (!SetupDiEnumDriverInfoW(set, &devData, SPDIT_CLASSDRIVER, idx++, &drv)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS) break;
            continue;
        }
        int score = driver_score(&drv);
        if ((DWORD)score > bestScore) {
            bestScore = (DWORD)score;
            g_bestDriver = drv;
        }
    }

    if (!bestScore) { err = 1168; goto cleanup; } // ERROR_NOT_FOUND
    newdev = LoadLibraryW(L"newdev.dll");
    if (!newdev) { err = GetLastError(); goto cleanup; }
    diInstallDevice = (PFN_DIINSTALLDEVICE)GetProcAddress(newdev, "DiInstallDevice");
    if (!diInstallDevice) { err = GetLastError(); goto cleanup; }

    if (!diInstallDevice(NULLPTR, set, &devData, &g_bestDriver, 0, &needReboot)) { err = GetLastError(); goto cleanup; }
    if (needReboot) *rebootNeeded = TRUE;
    err = 0;

cleanup:
    if (newdev) FreeLibrary(newdev);
    if (listBuilt) SetupDiDestroyDriverInfoList(set, &devData, SPDIT_CLASSDRIVER);
    if (set != INVALID_HANDLE_VALUE) SetupDiDestroyDeviceInfoList(set);
    return err;
}

void wWinMainCRTStartup(void) {
    if (!is_elevated_invocation()) {
        relaunch_elevated();
        return;
    }

    MessageBoxW(NULLPTR,
        L"Connect the USB floppy drive now, then click OK.\n\n"
        L"This installer uses the signed Microsoft USB floppy driver already included with Windows 10.",
        L"Viper Universal Windows 10 Floppy Driver", MB_OK | MB_ICONINFORMATION);

    WCHAR target[1024];
    target[0] = 0;
    if (!find_target_device(target, 1024)) {
        MessageBoxW(NULLPTR,
            L"No supported USB floppy drive was found.\n\n"
            L"Make sure the drive is plugged directly into the PC and appears in Device Manager.\n"
            L"The installer recognizes USB devices whose name contains Floppy, USB FDD, or SFD-321, plus the known Samsung VID_055D / PID_2020 device.",
            L"Viper Universal Windows 10 Floppy Driver", MB_OK | MB_ICONWARNING);
        return;
    }

    BOOL reboot = FALSE;
    DWORD err = install_inbox_floppy_driver(target, &reboot);
    if (err == 0) {
        if (reboot) {
            MessageBoxW(NULLPTR,
                L"Driver installed successfully.\n\nWindows reports that a restart is required. After rebooting, the floppy drive should appear as A: (or another drive letter).",
                L"Viper Floppy Driver Installed", MB_OK | MB_ICONINFORMATION);
        } else {
            MessageBoxW(NULLPTR,
                L"Driver installed successfully.\n\nUnplug the floppy drive, plug it back in, and check File Explorer. It should appear as A: (or another drive letter).",
                L"Viper Floppy Driver Installed", MB_OK | MB_ICONINFORMATION);
        }
    } else if (err == 1168) {
        MessageBoxW(NULLPTR,
            L"A USB floppy drive was detected, but this Windows 10 installation does not expose a compatible NEC/TEAC USB floppy entry in usbstor.inf.\n\n"
            L"No third-party or unsigned driver was installed. Try Windows Update, or use Device Manager -> Update driver -> Let me pick -> Show All Devices -> NEC Systems -> NEC USB Floppy.",
            L"Compatible Inbox Driver Not Found", MB_OK | MB_ICONWARNING);
    } else {
        MessageBoxW(NULLPTR,
            L"Windows detected the USB floppy drive, but the driver installation did not complete.\n\n"
            L"No unsigned driver was installed. Open Device Manager and verify the drive is still connected, then run this installer again.",
            L"Viper Floppy Driver Install Error", MB_OK | MB_ICONERROR);
    }
}
