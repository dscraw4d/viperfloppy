# v1.0.1 GitHub Build Hotfix

This fixes:

```text
lld-link: error: undefined symbol: memcpy
```

Replace these files in an existing v1.0.0 repository:

- `.github/workflows/build.yml`
- `build-local.cmd`
- `src/ViperUniversalFloppyDriver.c`
- `BUILDING.md`

Then commit and push. GitHub Actions will build with MSVC on `windows-2022` instead of the old LLVM/freestanding linker path.
