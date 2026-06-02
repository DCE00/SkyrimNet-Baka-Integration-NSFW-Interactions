<p align="center">
  <img src="logo.png" alt="SkyrimNet Baka Integration" width="640">
</p>

<h1 align="center">SkyrimNet Baka Integration — NSFW Interactions</h1>

<p align="center">
  <em>LLM-driven physical &amp; intimate interactions, facial expressions, and pose emotes for
  <a href="https://www.nexusmods.com/skyrimspecialedition/mods/146908">SkyrimNet</a>.</em>
</p>

> ⚠️ **Adult content (18+).** This addon adds non-consensual / NSFW interactions. Use responsibly.

---

## What it does

This is an addon for **SkyrimNet** — it lets the AI driving your NPCs choose, in context, to
perform physical and intimate actions, react with facial expressions, and strike body-language
poses during roleplay. It hooks into SkyrimNet through custom **actions, triggers, and decorators**.

*(add screenshots / a short demo clip here)*

## Features

- **Physical interactions** the LLM picks contextually:
  - Spanking — butt / face / breast slaps, with accumulating skin marks &amp; tattoos, impact sounds, and reactions
  - Grab hold, choke hold, struggle — paired animations with a resist QTE
  - Drug-food &amp; drunk exploit (incapacitate), womb hit
  - Forced kiss, fondle, touch / suck breasts, oral, examine / inspect
- **Escalation → SexLab** aggressive scenes, with defeat / bleedout &amp; recovery
- **Facial expressions** — happy / angry / afraid / sad / pained / surprised / confused
  - LLM-triggerable *and* automatic in-scene (fear in a struggle, pain on a choke / bleedout, sadness while crying)
  - Adjustable intensity
- **Pose emotes** — confident / casual / demure / seductive / presenting / kneeling, random-picked so NPCs vary
- **Reactions** — animated tears, face / tear overlays that survive sex scenes, cover-self after a spank
- **PrismaUI menus** for choosing interactions and setting up encounters

## Requirements

**Core**
- SkyrimNet (+ SKSE64, Address Library)
- [PrismaUI](https://www.nexusmods.com/skyrimspecialedition/mods/148718)
- PapyrusUtil, MfgFix, powerofthree's Papyrus Extender
- SexLab, SlaveTats
- EmoTears4NPCs (+ EmoTearsSpells)
- A Babo/SLAP animation pack + Hexed Poses, built with **FNIS / Nemesis / Pandora**

**Optional (degrades gracefully if absent)**
- Acheron, Flash Games – Struggling QTE, Dynamic Feminine Female Modesty Animations OAR

## Installation

1. Install all requirements above.
2. Install this mod with your mod manager (MO2/Vortex), let it win conflicts for its own files.
3. Run **FNIS / Nemesis / Pandora** to generate the bundled paired animations &amp; poses.
4. Launch once so SkyrimNet loads the bundled action configs (`SKSE/Plugins/SkyrimNet/config/`).

## Configuration

MCM (and script properties) expose toggles:
- `bExpressionsEnabled`, `bPosesEnabled` — feature master switches
- `fExpressionIntensity` (0.0–1.0) — how strong faces look
- `fPoseHoldTime` — seconds a pose is held
- spank cooldowns, male-target / player-target allowances, animated tears, etc.

## Credits

- **SkyrimNet** — the framework this builds on
- Paired interaction animations — *Babo / SLAP* animation authors
- Pose emotes — **Hexed Poses**
- Cover-self reaction — driven by the *Dynamic Feminine Female Modesty Animations OAR* mod (Kahvipannu84 / Gunslicer); install it for that feature (no animations are bundled here)
- Facial-expression morph values — *Additional Expressions Project*
- Frameworks — SexLab, PrismaUI, PapyrusUtil, MfgFix, po3 Papyrus Extender, SlaveTats, EmoTears4NPCs, Acheron

> Bundled third-party animations/assets remain the property of their original authors and are
> included per their permissions. If you are an author and want something removed, open an issue.

## Links

- Nexus: **[add link]**
- Discord: **[add link]**
