## New: the downed-victim menu

Press the power on a victim you've knocked down and you now get a small grid menu instead of an instant escalation:
- **Choke Down** -- straddle the grounded victim into the SexLab scene (the old auto-escalate).
- **Investigate** / **Inspect** -- play the full inspection animation, then drop them back down and reset the timer (they stay defeated).
- **Stand Back** -- the victim staggers, gets up, and you regain control.
- **Esc** -- leave them down; the window keeps running.

## Menu reorganized

- Tabs are now **Affection / Forced / Sexual** (matching how the offsets file is grouped) instead of the old four.
- The action grid is now **near-square** (columns scale with the count) so long lists need much less scrolling.
- New affectionate action: **Arm Hold** -- take a partner gently by the arm. Tunable in `SNBaka_Offsets.ini` (key `BaboHoldArmM`), and available to NPCs/SkyrimNet too.

## Fixes

- **MCM "Player Can Be Target"** now only stops NPCs from targeting *you*. Before, unchecking it also blocked every action *you* started -- which looked like the whole mod was turned off.
- **Investigate / Inspect / Play Privates**: no more QTE -- they just play.
- **Investigate / Inspect from the main menu no longer knock the victim down** (that only happens via the downed menu now).
- **Downed-menu picks no longer occasionally do nothing** (a flag-timing race right after a knockdown).
- **Affectionate interactions clear any leftover tears / crying face** from an earlier forced scene, so a hug or kiss never shows the partner still sobbing.
- **Investigation** first stage now turns only the aggressor 180 degrees (the rest already looked right).

## Install

- Install / overwrite with your mod manager, after the SexLab / SkyrimNet frameworks.
- **Restart Skyrim once** -- this version updates the PrismaUI menu HTML and the scripts (both load at launch). After that, editing `SNBaka_Offsets.ini` still applies on a save reload.
