; Papyrus stub for SNBaka_UI.dll (SKSE plugin).
; If the DLL is absent all three functions are no-ops / return false,
; so the quest script falls back to vanilla Message.Show() menus.
Scriptname SNBakaUI

; Returns true when PrismaUI is loaded and the menu view is ready.
Bool Function IsAvailable() Global Native

; Show the interact menu panel.  The result is returned asynchronously via
; the SKSE mod event "SNBaka_MenuChoice" (strArg="interact", numArg=choiceId).
;   0=BackHug  1=FrontHug  2=Kiss  3=Flirt
;   4=GrabHold 5=Struggle  6=Choke  7=WombHit
;   8=ForcedKiss 9=Spank 10=TouchChest 11=Examine
;  12=ShowOffBody 13=DrunkExploit 14=DrugFood 15=Fondle
;  -1=Cancel
Function ShowInteractMenu(Actor akCaster, Actor akTarget) Global Native

; Resolves the NPC the player is targeting (crosshair, then nearest-actor fallback).
; Returns None if nobody is targeted.  Used because the interact power is self-delivered,
; so the magic effect's akTarget is always the player.
Actor Function GetInteractTarget() Global Native

; Disable/restore character-to-character collision on an actor (havok layer swap).
; Called for both participants at animation start and restored in _CleanupPair so
; actors can be positioned overlapping without the physics solver shoving them.
Function SetNoCollision(Actor akActor, Bool abDisable) Global Native

; True if the furniture is an alchemy lab or enchanting table.  Used to make a
; female bent over one of those a high-temptation spank target for SkyrimNet.
Bool Function IsCraftingTemptation(ObjectReference akFurniture) Global Native

; Opens the multi-step SexLab encounter setup wizard (Roles -> Intensity ->
; Flavor -> Type).  The player's picks come back as "role;intensity;flavor;type"
; (or "cancel") and are passed to SkyrimNet_BakaIntegration._StartSexLabScene
; together with these two actors.  Only call when the player is involved.
Function ShowEncounterMenu(Actor akAggressor, Actor akVictim) Global Native

; Show the sex-spank menu panel.  json must be:
;   {"names":["Lydia","Serana"],"playerInScene":true}
; Result via "SNBaka_MenuChoice" (strArg="sexspank", numArg=choiceId).
;   0/1/2   = spank sceneNPCs[0/1/2]
;   10/11/12 = sceneNPCs[0/1/2] spank player
;   13      = player self-spank
;  -1       = cancel
Function ShowSexSpankMenu(String json) Global Native
