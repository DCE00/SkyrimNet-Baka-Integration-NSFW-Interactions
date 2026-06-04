# SNBaka_UI — Build Instructions

## Prerequisites

1. **Visual Studio 2022** (Community edition is fine)
   - Workload: "Desktop development with C++"
   - Component: "C++ CMake tools for Windows"

2. **vcpkg**  
   If you don't have it yet:
   ```
   git clone https://github.com/microsoft/vcpkg C:\vcpkg
   C:\vcpkg\bootstrap-vcpkg.bat
   ```
   Then set the environment variable permanently:
   ```
   setx VCPKG_ROOT C:\vcpkg
   ```
   (Open a new terminal after setting it.)

3. **Ninja** (bundled with Visual Studio — nothing extra needed)

## Build steps

Open a **Developer Command Prompt for VS 2022** (or any terminal where `cmake` and `cl.exe` are on `PATH`).

```
cd path\to\SNBaka_UI_Plugin
cmake --preset release
cmake --build --preset release
```

The DLL is copied to the mod's plugins folder by a post-build step (see `CMakeLists.txt` — adjust
the destination path there to your own mod install):
```
<your SkyrimNet_BakaIntegration mod folder>\SKSE\Plugins\SNBaka_UI.dll
```

## What happens after you install it

1. **Launch Skyrim via MO2** (the new DLL gets loaded).
2. The DLL requests the PrismaUI API, creates the `SNBaka_Menu` HTML view (hidden).
3. When you use the Interact power on an NPC the Papyrus script calls
   `SNBakaUI.IsAvailable()` → true → calls `SNBakaUI.ShowInteractMenu(targetName)`.
4. The C++ DLL invokes `window.snbaka_open_interact('Name')` on the HTML panel,
   focuses the view, and pauses the game.
5. You click an action in the panel.  JS calls `window.snbaka_chose("3")`.
6. The C++ JS listener fires, sends SKSE mod event `SNBaka_MenuChoice`
   (strArg="interact", numArg=3.0) to Papyrus.
7. `OnSNBakaMenuChoice` calls `_DispatchInteractAction(3)` → `Flirt_Execute()`.

If the DLL is absent or PrismaUI.dll is missing, `SNBakaUI.IsAvailable()` returns
false and the script falls back to the vanilla messagebox menus automatically.

## Action ID mapping

| ID  | Action            |
|-----|-------------------|
| 0   | Back Hug          |
| 1   | Front Hug         |
| 2   | Kiss              |
| 3   | Flirt             |
| 4   | Grab Hold         |
| 5   | Struggle          |
| 6   | Choke Hold        |
| 7   | Womb Hit          |
| 8   | Forced Kiss       |
| 9   | Spank             |
| 10  | Touch Chest       |
| 11  | Examine           |
| 12  | Show Off Body     |
| 13  | Drunk Exploit     |
| 14  | Drug Food         |
| 15  | Fondle            |
| -1  | Cancel (no-op)    |
