Scriptname SkyrimNet_BakaEscalateEffect extends ActiveMagicEffect
{ Magic effect script for the Baka Escalate lesser power.
  Fires Escalate_Execute on the targeted actor. Silently no-ops if that
  actor is not currently in the ground window (SNBaka.OnGround != 1). }

SkyrimNet_BakaIntegration Property BakaMain Auto

Event OnEffectStart(Actor akTarget, Actor akCaster)
    If !BakaMain || !akTarget || !akCaster
        Return
    EndIf
    BakaMain.Escalate_Execute(akCaster, akTarget)
EndEvent
