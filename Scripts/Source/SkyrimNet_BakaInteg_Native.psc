; Native collision helpers implemented in SNBaka_Collision.dll (SKSE plugin).
; If the DLL is not installed these calls are no-ops — the mod still works,
; actors just won't have their physics collision suppressed during animations.
Scriptname SkyrimNet_BakaInteg_Native Hidden

; Disable Havok collision between akA1 and akA2.
; Call this immediately before SetVehicle/MoveTo in paired animations.
Function DisablePairCollision(Actor akA1, Actor akA2) Global Native

; Restore the collision layer saved by DisablePairCollision.
; Call this in _CleanupPair after ClearVehicle.
Function EnablePairCollision(Actor akA1, Actor akA2) Global Native

; Restore collision for every actor currently in the saved-filter map.
; Call on OnPlayerLoadGame and EmergencyReset to prevent permanently-disabled collision
; after a mid-animation cell change, load, or script crash.
Function RestoreAllCollision() Global Native
