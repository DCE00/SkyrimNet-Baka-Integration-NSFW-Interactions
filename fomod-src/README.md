# FOMOD installer source

This folder builds the **FOMOD installer** release archive — the one that lets users pick
their Skyrim runtime at install time. It is **excluded from the normal data-root release zip**
(`.gitattributes` `export-ignore`), so it never ships inside the plain mod files.

## Layout produced by the installer
```
fomod/                 ← info.xml + ModuleConfig.xml (the installer UI/logic)
core/                  ← ALL shared mod files (Scripts, SKSE/.../config, textures, ESP, PrismaUI…)
                          — everything EXCEPT the SKSE DLL
plugin-seae/SKSE/Plugins/SNBaka_UI.dll   ← the proven SE/AE DLL (stable option)
plugin-vr/  SKSE/Plugins/HOW_TO_GET_VR_DLL.txt  ← empty VR slot (experimental option)
```
- **SE / AE** option → `core` + `plugin-seae` (full, tested install).
- **VR** option → `core` + `plugin-vr` (everything except the DLL; a VR dev drops a built
  `SNBaka_UI.dll` into `plugin-vr/SKSE/Plugins/` to complete it).

The SE/AE DLL is **never modified** — the VR option only swaps which plugin folder installs.

## Building the FOMOD zip
From the repo root, after committing your changes:
```
bash fomod-src/build-fomod.sh            # -> SkyrimNet_BakaIntegration_FOMOD.zip
```
`core/` is taken from `git archive HEAD` (so it matches the tracked release and already
excludes `dll-source/` and `fomod-src/`), minus the DLL. `plugin-seae/` gets the current
`SKSE/Plugins/SNBaka_UI.dll`.

## Completing the VR build (for modders)
Build `SNBaka_UI.dll` for VR from `dll-source/` (see `dll-source/BUILD.md`), drop it into
`fomod-src/plugin-vr/SKSE/Plugins/`, delete the `HOW_TO_GET_VR_DLL.txt` placeholder, rebuild
the FOMOD zip. Done.
