; Attached to ButtReactionSpell or BreastReactionSpell MGEF.
; Adds a keyword on effect start, removes it when the duration expires.
; OAR reads the keyword and plays the covering idle/walk animations.

Scriptname SkyrimNet_BakaInteg_ModestyEffect extends ActiveMagicEffect

Keyword Property SpankedKeyword Auto

Event OnEffectStart(Actor akTarget, Actor akCaster)
    If SpankedKeyword && akTarget
        akTarget.AddKeyword(SpankedKeyword)
    EndIf
EndEvent

Event OnEffectFinish(Actor akTarget, Actor akCaster)
    If SpankedKeyword && akTarget
        akTarget.RemoveKeyword(SpankedKeyword)
    EndIf
EndEvent
