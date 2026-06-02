; Cast by script on a spanked NPC after a breast slap (non-sex).  Female NPCs only —
; the player is never a breast-slap target (guarded in BreastSlap_Execute), so unlike
; the butt reaction this only needs the NPC force-on keyword.
;
; Adds the "Dynamic Feminine Female Modesty Animations OAR" force-on keyword
; (ModestyDetectionForceOnNPC = 0xD90 in Modesty_Keyword.esp) so its cover-self idle
; plays, then removes it after a random 5-8 s.  SOFT dependency: if that esp isn't
; installed the keyword resolves to None and we no-op.  AddKeyword/RemoveKeyword need
; po3 PapyrusExtender.  idleforcedefaultstate kicks OAR into re-selecting the idle.
; A sequence counter stops an earlier slap's timer from clearing a later one.

Scriptname SkyrimNet_BakaInteg_BreastReaction_ME extends ActiveMagicEffect

Event OnEffectStart(Actor akTarget, Actor akCaster)
    If !akTarget || akTarget.GetActorBase().GetSex() != 1
        Return
    EndIf
    If akTarget.IsDead() || akTarget.IsInCombat()
        Return
    EndIf

    Keyword forceOn = Game.GetFormFromFile(0x000D90, "Modesty_Keyword.esp") as Keyword
    If !forceOn
        Debug.Trace("[SNBaka] BreastReaction: Modesty_Keyword.esp not found — no cover-self idle.")
        Return
    EndIf

    akTarget.AddKeyword(forceOn)

    Int mySeq = StorageUtil.GetIntValue(akTarget, "SkyrimNetSDB.BreastSpankSeq", 0) + 1
    StorageUtil.SetIntValue(akTarget, "SkyrimNetSDB.BreastSpankSeq", mySeq)
    Debug.Trace("[SNBaka] BreastReaction: cover-self ON for " + akTarget.GetDisplayName() + " (seq " + mySeq + ")")

    ; Kick OAR into re-selecting the idle now that the keyword is present.
    Utility.Wait(1.2)
    If akTarget.GetFurnitureReference() == None
        Debug.SendAnimationEvent(akTarget, "idleforcedefaultstate")
    EndIf

    ; Hold the cover pose for a random 5-8 s.
    Utility.Wait(5.0 + Utility.RandomFloat(0.0, 3.0))

    ; Only the most recent slap's timer is allowed to clear.
    If StorageUtil.GetIntValue(akTarget, "SkyrimNetSDB.BreastSpankSeq", 0) == mySeq
        akTarget.RemoveKeyword(forceOn)
        If akTarget.GetFurnitureReference() == None
            Debug.SendAnimationEvent(akTarget, "idleforcedefaultstate")
        EndIf
        Debug.Trace("[SNBaka] BreastReaction: cover-self cleared for " + akTarget.GetDisplayName())
    EndIf
EndEvent
