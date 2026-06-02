Scriptname SkyrimNet_BakaInteractEffect extends ActiveMagicEffect
{ Interact power: spank during sex, escalate on bleedout, menu otherwise. }

SkyrimNet_BakaIntegration Property BakaMain Auto

Event OnEffectStart(Actor akTarget, Actor akCaster)
    If !BakaMain || !akCaster
        Debug.Notification("[SNBaka] InteractEffect: BakaMain or caster None")
        Return
    EndIf

    ; The interact power is self-delivered, so akTarget is always the player.
    ; In a sex scene we don't need a target — the spank menu finds partners itself.
    If BakaMain.IsInSexAnimation(akCaster)
        BakaMain.SexSpank_ShowMenu(akCaster)
        Return
    EndIf

    ; Otherwise resolve the real NPC the player is aiming at (crosshair / nearest).
    Actor realTarget = SNBakaUI.GetInteractTarget()
    If !realTarget
        Return
    EndIf

    ; Downed victim -> escalate (choke -> sex).  Both the bleedout state and the
    ; SNBaka.OnGround flag (checked inside Interact_ShowMenu) route here.
    If realTarget.IsBleedingOut()
        BakaMain.Escalate_Execute(akCaster, realTarget)
        Return
    EndIf
    BakaMain.Interact_ShowMenu(realTarget, akCaster)
EndEvent
