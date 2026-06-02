; Self-delivery "Spank Partner" lesser power.
; During sex  → spanks sex partner (sound + marks, no animation).
; Outside sex → spanks nearest NPC within 800 units (full animation).
; Self delivery means this always fires during SexLab — no crosshair needed.

Scriptname SkyrimNet_BakaInteg_SpankPartner extends ActiveMagicEffect

SkyrimNet_BakaIntegration Property MainQuest Auto

Event OnEffectStart(Actor akTarget, Actor akCaster)
    Debug.Trace("[SNBaka_SpankPartner] OnEffectStart — akCaster=" + akCaster + " akTarget=" + akTarget)
    If !MainQuest
        MainQuest = Game.GetFormFromFile(0x000D62, "SkyrimNet_BakaIntegration.esp") as SkyrimNet_BakaIntegration
        Debug.Trace("[SNBaka_SpankPartner] MainQuest auto-resolved: " + MainQuest)
    EndIf
    If !MainQuest
        Debug.Notification("[SNBaka] SpankPartner: could not resolve MainQuest — check ESP is active.")
        Return
    EndIf
    If !MainQuest.bEnabled
        Debug.Notification("[SNBaka] SpankPartner: mod disabled.")
        Return
    EndIf
    If akCaster != Game.GetPlayer()
        Debug.Trace("[SNBaka_SpankPartner] caster is not player — ignoring")
        Return
    EndIf
    If akCaster.IsDead()
        Debug.Notification("[SNBaka] SpankPartner: caster is dead.")
        Return
    EndIf

    Actor spankTarget = akTarget
    If spankTarget == akCaster || spankTarget == None
        spankTarget = MainQuest.FindSexPartner(akCaster)
    EndIf

    If spankTarget != None
        Debug.Notification("[SNBaka] Spanking: " + spankTarget.GetDisplayName())
        MainQuest.SpankTarget_Execute(akCaster, spankTarget, True)
        String sDesc = akCaster.GetDisplayName() + " spanked " + spankTarget.GetDisplayName() + "."
        SkyrimNetApi.DirectNarration(sDesc, akCaster, spankTarget)
    Else
        Debug.Notification("[SNBaka] SpankPartner: no target.")
    EndIf
EndEvent
