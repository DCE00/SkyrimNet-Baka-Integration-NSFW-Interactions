#!/usr/bin/env bash
# Assemble the FOMOD installer archive for SkyrimNet Baka Integration.
# Run from anywhere; resolves the repo as the parent of this script's folder.
#
#   core/        = git-archived release files (already excludes dll-source/ + fomod-src/) MINUS the DLL
#   plugin-seae/ = the proven SE/AE SNBaka_UI.dll
#   plugin-vr/   = the experimental VR slot (HOW_TO_GET_VR_DLL.txt placeholder, or a built VR DLL)
#   fomod/       = the installer definition
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/.." && pwd)"
OUT="${1:-$REPO/SkyrimNet_BakaIntegration_FOMOD.zip}"

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
echo "[fomod] staging in $STAGE"

# 1) core = the tracked release files (export-ignore already drops dll-source/ + fomod-src/), minus the DLL
mkdir -p "$STAGE/core"
git -C "$REPO" archive HEAD | tar -x -C "$STAGE/core"
rm -f "$STAGE/core/SKSE/Plugins/SNBaka_UI.dll"
rm -f "$STAGE/core/.gitignore" "$STAGE/core/.gitattributes"

# 2) plugin-seae = the proven SE/AE DLL
mkdir -p "$STAGE/plugin-seae/SKSE/Plugins"
cp "$REPO/SKSE/Plugins/SNBaka_UI.dll" "$STAGE/plugin-seae/SKSE/Plugins/"

# 3) plugin-vr = the VR slot scaffold (or a dropped-in VR DLL)
cp -r "$SCRIPT_DIR/plugin-vr" "$STAGE/plugin-vr"

# 4) fomod definition
cp -r "$SCRIPT_DIR/fomod" "$STAGE/fomod"

# 5) zip — prefer `zip`. Otherwise zip via a throwaway git repo: `git archive` writes a
# spec-compliant zip with FORWARD-SLASH paths (PowerShell 5.1 Compress-Archive writes
# backslashes, which breaks FOMOD parsers — do NOT use it here).
rm -f "$OUT"
if command -v zip >/dev/null 2>&1; then
    ( cd "$STAGE" && zip -r -q "$OUT" . )
else
    git -C "$STAGE" init -q
    git -C "$STAGE" add -A
    git -C "$STAGE" -c user.email=pkg@local -c user.name=pkg commit -qm package >/dev/null
    git -C "$STAGE" archive --format=zip -o "$OUT" HEAD
    rm -rf "$STAGE/.git"
fi
echo "[fomod] built -> $OUT"
