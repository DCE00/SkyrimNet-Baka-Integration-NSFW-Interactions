; Cast by script on the spanked NPC after a butt slap (non-sex).  Female-only.
;
; Adds the "Dynamic Feminine Female Modesty Animations OAR" force-on keyword so its
; cover-self idle plays, then removes it after a random 5-8 s.  The OAR submods (and
; therefore the codewords) are split PC vs NPC, so we pick the matching keyword:
;   ModestyDetectionForceOnNPC = 0xD90   (NPCs)
;   ModestyDetectionForceOnPC  = 0xD8F   (the player)
; Both live in Modesty_Keyword.esp.  This is a SOFT dependency: if that esp isn't
; installed the keyword resolves to None and we no-op (no error).
;
; NOTE: the force-on keyword forces modesty DETECTION on; which pose plays (butt vs
; chest) is still decided by OAR from what the actor has exposed, so on a clothed NPC
; the exact cover can vary.  idleforcedefaultstate kicks OAR into re-selecting the idle
; once the keyword is present (and again when it's removed).  AddKeyword/RemoveKeyword
; need po3 PapyrusExtender.  A sequence counter stops an earlier spank's timer from
; clearing a later one.

Scriptname SkyrimNet_BakaInteg_ButtReaction_ME extends ActiveMagicEffect

Event OnEffectStart(Actor akTarget, Actor akCaster)
    If !akTarget || akTarget.GetActorBase().GetSex() != 1
        Return
    EndIf
    If akTarget.IsDead() || akTarget.IsInCombat()
        Return
    EndIf

    Keyword forceOn = _ModestyKeyword(akTarget)
    If !forceOn
        Debug.Trace("[SNBaka] ButtReaction: Modesty_Keyword.esp not found — no cover-self idle.")
        Return
    EndIf

    Bool isPlayer = (akTarget == Game.GetPlayer())
    akTarget.AddKeyword(forceOn)

    Int mySeq = StorageUtil.GetIntValue(akTarget, "SkyrimNetSDB.ButtSpankSeq", 0) + 1
    StorageUtil.SetIntValue(akTarget, "SkyrimNetSDB.ButtSpankSeq", mySeq)
    Debug.Trace("[SNBaka] ButtReaction: cover-self ON for " + akTarget.GetDisplayName() + " (seq " + mySeq + ")")

    ; Kick OAR into re-selecting the idle now that the keyword is present.
    Utility.Wait(1.2)
    If !isPlayer || akTarget.GetFurnitureReference() == None
        Debug.SendAnimationEvent(akTarget, "idleforcedefaultstate")
    EndIf

    ; Hold the cover pose for a random 5-8 s.
    Utility.Wait(5.0 + Utility.RandomFloat(0.0, 3.0))

    ; Only the most recent spank's timer is allowed to clear.
    If StorageUtil.GetIntValue(akTarget, "SkyrimNetSDB.ButtSpankSeq", 0) == mySeq
        akTarget.RemoveKeyword(forceOn)
        If !isPlayer || akTarget.GetFurnitureReference() == None
            Debug.SendAnimationEvent(akTarget, "idleforcedefaultstate")
        EndIf
        Debug.Trace("[SNBaka] ButtReaction: cover-self cleared for " + akTarget.GetDisplayName())
    EndIf
EndEvent

; ForceOnPC for the player, ForceOnNPC for everyone else — resolved at runtime so
; Modesty_Keyword.esp stays an optional dependency.
Keyword Function _ModestyKeyword(Actor akTarget)
    If akTarget == Game.GetPlayer()
        Return Game.GetFormFromFile(0x000D8F, "Modesty_Keyword.esp") as Keyword
    EndIf
    Return Game.GetFormFromFile(0x000D90, "Modesty_Keyword.esp") as Keyword
EndFunction
