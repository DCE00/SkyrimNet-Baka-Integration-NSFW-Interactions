## v1.9.1 — EXPERIMENTAL

> This is an experimental release: a new NPC body-language system built from previously-unused
> Baka Motion Data Pack animations. Please test and report anything that misbehaves.

## New: NPC poses & body language (LLM-driven)

A new action category — **`SNBaka_Pose`** — gives NPCs **15 solo poses** that SkyrimNet can use on its
own in fitting moments (one token-cheap category, so it doesn't bloat the LLM). Each plays, holds, and
releases cleanly, and bows out if a fight starts (or the actor is hit, teleports, or dies):

- **Submission:** Kneel, Dogeza (deep grovelling bow)
- **Idle:** Crouch, Sleep, Meditate
- **Gesture:** Scratch head, Brace own arm, Hand on face, Hand on chin, Drool (dazed)
- **Flavor:** Aroused idle (gendered), Autograph (mimed), Pickpocket (mimed)
- **Women only:** Drink — with a chance to drink herself into a helpless, passed-out stupor (which
  signals nearby characters that she's vulnerable) — and Food (eats anyway / recoils in disgust)

## New: defeat-pose variety

Knocked-down victims no longer always collapse the same way — they now drop into one of several
defeated poses at random (lying trauma, knocked out cold, or a faint for women), so repeated
takedowns look less identical.

## Tuning

- `fSoloPoseDuration` (default 30s) — how long a held pose lasts.
- `iDrinkBlackoutChance` (default 30) — % chance the Drink pose ends in a drunken blackout.

## Notes / known caveats

- **Some poses need Nemesis.** Kneel, Dogeza, Crouch, Drink, Food and the Aroused/defeat poses work
  under FNIS or Nemesis. Meditate, the hand/head gestures, Sleep, Autograph and Pickpocket live on the
  Nemesis animation list — they only play if your setup runs the BaboMotionData **Nemesis** patch.
- The drink blackout currently just flags the opening (a "helpless" cue) for a nearby NPC or your power
  to act on; it does not auto-start an escalation.

## Install

- Install / overwrite with your mod manager, after the SexLab / SkyrimNet frameworks and the Baka
  Motion Data Pack.
- **Restart Skyrim once** — this version updates the SKSE DLL (new action category) and the SkyrimNet
  action configs, both of which load at launch.
