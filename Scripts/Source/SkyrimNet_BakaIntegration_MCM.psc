Scriptname SkyrimNet_BakaIntegration_MCM extends SKI_ConfigBase

SkyrimNet_BakaIntegration Property Main Auto

Int _enabledOID
Int _playerTargetOID
Int _femaleOnlyOID
Int _animatedTearsOID
Int _hugDurOID
Int _molestDurOID
Int _kissDurOID
Int _touchDurOID
Int _stageDurOID
Int _cooldownOID
Int _resistEnabledOID
Int _resistDifficultyOID
Int _escalationWindowOID
Int _escalationDifficultyOID
Int _spankPlayerOID
Int _spankMaleOID
Int _spankFurnitureOID
Int _spankTatIntensityOID
Int _spankHealFactorOID
Int _sexBackendOID
Int _expressionsOID
Int _exprIntensityOID

Event OnConfigInit()
    ModName = "Baka SkyrimNet"
    _BuildPages()
EndEvent

; Bumped to 2 in v1.1 (added the "Scenes & FX" page).  Increasing this makes SkyUI fire
; OnVersionUpdate on existing saves so the new page actually appears without a console refresh.
Int Function GetVersion()
    Return 2
EndFunction

Event OnVersionUpdate(Int aVersion)
    _BuildPages()
EndEvent

Function _BuildPages()
    Pages    = new String[5]
    Pages[0] = "General"
    Pages[1] = "Timing"
    Pages[2] = "Resist"
    Pages[3] = "Spank"
    Pages[4] = "Scenes & FX"
EndFunction

Event OnPageReset(String page)
    If page == "General"
        SetCursorFillMode(TOP_TO_BOTTOM)
        _enabledOID        = AddToggleOption("Enable Mod",            Main.bEnabled)
        _playerTargetOID   = AddToggleOption("Player Can Be Target",  Main.bPlayerCanBeTarget)
        _femaleOnlyOID     = AddToggleOption("Female Targets Only",   Main.bFemaleTargetOnly)
        _animatedTearsOID  = AddToggleOption("Animated Tears",        Main.bAnimatedTearsEnabled)
        _cooldownOID       = AddSliderOption("AI Action Cooldown",    Main.fNPCGlobalCooldown, "{0}s")
    ElseIf page == "Timing"
        SetCursorFillMode(TOP_TO_BOTTOM)
        _hugDurOID    = AddSliderOption("Hug Loop Duration",         Main.fHugLoopDuration,    "{0}s")
        _molestDurOID = AddSliderOption("Molest Loop Duration",      Main.fMolestLoopDuration, "{0}s")
        _kissDurOID   = AddSliderOption("Kiss Duration (per stage)", Main.fKissLoopDuration,   "{0}s")
        _touchDurOID  = AddSliderOption("Touch Duration",            Main.fTouchLoopDuration,  "{0}s")
        _stageDurOID  = AddSliderOption("Sequence Stage Duration",   Main.fSequenceStageTimer, "{0}s")
    ElseIf page == "Resist"
        SetCursorFillMode(TOP_TO_BOTTOM)
        _resistEnabledOID        = AddToggleOption("Enable Resist Minigame",    Main.bResistEnabled)
        _resistDifficultyOID     = AddSliderOption("Escape Difficulty",          Main.fResistDifficulty,    "{0}%")
        AddTextOption("QTE Keys", "Configured in Flash Games - Struggling QTE  (WASD / d-pad)", OPTION_FLAG_DISABLED)
        AddEmptyOption()
        _escalationWindowOID     = AddSliderOption("Defeat: Escalation Window",  Main.fEscalationWindow,    "{0}s")
        _escalationDifficultyOID = AddSliderOption("Defeat: Escalation QTE",     Main.fEscalationDifficulty, "{0}%")
    ElseIf page == "Spank"
        SetCursorFillMode(TOP_TO_BOTTOM)
        AddHeaderOption("Behaviour")
        _spankPlayerOID    = AddToggleOption("Player Can Be Spanked",  Main.bPlayerCanBeSpanked)
        _spankMaleOID      = AddToggleOption("Allow Male Targets",     Main.bSpankMaleTargets)
        _spankFurnitureOID = AddToggleOption("Furniture Reactions",    Main.bSpankFurnitureTriggers)
        AddEmptyOption()
        AddHeaderOption("Tattoo Marks")
        _spankTatIntensityOID = AddSliderOption("Spanks Per Stage", Main.SpankTatIntensity as Float, "{0}")
        _spankHealFactorOID   = AddSliderOption("Hours Per Stage",  Main.SpankHealFactor as Float,   "{0} hr")
    ElseIf page == "Scenes & FX"
        SetCursorFillMode(TOP_TO_BOTTOM)
        AddHeaderOption("Sex Framework")
        _sexBackendOID    = AddMenuOption("Scene Framework", _backendName(Main.iSexBackend))
        AddEmptyOption()
        AddHeaderOption("Expressions")
        _expressionsOID   = AddToggleOption("Facial Expressions",  Main.bExpressionsEnabled)
        _exprIntensityOID = AddSliderOption("Expression Intensity", Main.fExpressionIntensity, "{2}")
    EndIf
EndEvent

Event OnOptionSelect(Int option)
    If option == _enabledOID
        Main.bEnabled = !Main.bEnabled
        SetToggleOptionValue(_enabledOID, Main.bEnabled)
    ElseIf option == _animatedTearsOID
        Main.bAnimatedTearsEnabled = !Main.bAnimatedTearsEnabled
        SetToggleOptionValue(_animatedTearsOID, Main.bAnimatedTearsEnabled)
        ; If just enabled and spell not yet resolved, try now
        If Main.bAnimatedTearsEnabled && !Main.TearSpell
            Main.TearSpell = Game.GetFormFromFile(0x000802, "EmoTears4NPCs.esp") as Spell
        EndIf
    ElseIf option == _playerTargetOID
        Main.bPlayerCanBeTarget = !Main.bPlayerCanBeTarget
        SetToggleOptionValue(_playerTargetOID, Main.bPlayerCanBeTarget)
    ElseIf option == _femaleOnlyOID
        Main.bFemaleTargetOnly = !Main.bFemaleTargetOnly
        SetToggleOptionValue(_femaleOnlyOID, Main.bFemaleTargetOnly)
    ElseIf option == _resistEnabledOID
        Main.bResistEnabled = !Main.bResistEnabled
        SetToggleOptionValue(_resistEnabledOID, Main.bResistEnabled)
    ElseIf option == _spankPlayerOID
        Main.bPlayerCanBeSpanked = !Main.bPlayerCanBeSpanked
        SetToggleOptionValue(_spankPlayerOID, Main.bPlayerCanBeSpanked)
    ElseIf option == _spankMaleOID
        Main.bSpankMaleTargets = !Main.bSpankMaleTargets
        SetToggleOptionValue(_spankMaleOID, Main.bSpankMaleTargets)
    ElseIf option == _spankFurnitureOID
        Main.bSpankFurnitureTriggers = !Main.bSpankFurnitureTriggers
        SetToggleOptionValue(_spankFurnitureOID, Main.bSpankFurnitureTriggers)
    ElseIf option == _expressionsOID
        Main.bExpressionsEnabled = !Main.bExpressionsEnabled
        SetToggleOptionValue(_expressionsOID, Main.bExpressionsEnabled)
    EndIf
EndEvent

Event OnOptionMenuOpen(Int option)
    If option == _sexBackendOID
        SetMenuDialogOptions(_backendNames())
        SetMenuDialogStartIndex(Main.iSexBackend)
        SetMenuDialogDefaultIndex(0)
    EndIf
EndEvent

Event OnOptionMenuAccept(Int option, Int index)
    If option == _sexBackendOID
        Main.iSexBackend = index
        SetMenuOptionValue(_sexBackendOID, _backendName(index))
    EndIf
EndEvent

String[] Function _backendNames()
    String[] a = new String[3]
    a[0] = "Auto"
    a[1] = "SexLab"
    a[2] = "OStim"
    Return a
EndFunction

String Function _backendName(Int i)
    If i == 1
        Return "SexLab"
    ElseIf i == 2
        Return "OStim"
    EndIf
    Return "Auto"
EndFunction

Event OnOptionSliderOpen(Int option)
    If option == _hugDurOID
        SetSliderDialogStartValue(Main.fHugLoopDuration)
        SetSliderDialogDefaultValue(8.0)
        SetSliderDialogRange(2.0, 30.0)
        SetSliderDialogInterval(1.0)
    ElseIf option == _molestDurOID
        SetSliderDialogStartValue(Main.fMolestLoopDuration)
        SetSliderDialogDefaultValue(8.0)
        SetSliderDialogRange(2.0, 30.0)
        SetSliderDialogInterval(1.0)
    ElseIf option == _kissDurOID
        SetSliderDialogStartValue(Main.fKissLoopDuration)
        SetSliderDialogDefaultValue(6.0)
        SetSliderDialogRange(2.0, 20.0)
        SetSliderDialogInterval(1.0)
    ElseIf option == _touchDurOID
        SetSliderDialogStartValue(Main.fTouchLoopDuration)
        SetSliderDialogDefaultValue(6.0)
        SetSliderDialogRange(2.0, 20.0)
        SetSliderDialogInterval(1.0)
    ElseIf option == _stageDurOID
        SetSliderDialogStartValue(Main.fSequenceStageTimer)
        SetSliderDialogDefaultValue(4.0)
        SetSliderDialogRange(1.0, 15.0)
        SetSliderDialogInterval(0.5)
    ElseIf option == _cooldownOID
        SetSliderDialogStartValue(Main.fNPCGlobalCooldown)
        SetSliderDialogDefaultValue(30.0)
        SetSliderDialogRange(0.0, 120.0)
        SetSliderDialogInterval(5.0)
    ElseIf option == _resistDifficultyOID
        SetSliderDialogStartValue(Main.fResistDifficulty)
        SetSliderDialogDefaultValue(70.0)
        SetSliderDialogRange(10.0, 95.0)
        SetSliderDialogInterval(5.0)
    ElseIf option == _escalationWindowOID
        SetSliderDialogStartValue(Main.fEscalationWindow)
        SetSliderDialogDefaultValue(20.0)
        SetSliderDialogRange(5.0, 60.0)
        SetSliderDialogInterval(5.0)
    ElseIf option == _escalationDifficultyOID
        SetSliderDialogStartValue(Main.fEscalationDifficulty)
        SetSliderDialogDefaultValue(70.0)
        SetSliderDialogRange(10.0, 95.0)
        SetSliderDialogInterval(5.0)
    ElseIf option == _spankTatIntensityOID
        SetSliderDialogStartValue(Main.SpankTatIntensity as Float)
        SetSliderDialogDefaultValue(2.0)
        SetSliderDialogRange(1.0, 10.0)
        SetSliderDialogInterval(1.0)
    ElseIf option == _spankHealFactorOID
        SetSliderDialogStartValue(Main.SpankHealFactor as Float)
        SetSliderDialogDefaultValue(2.0)
        SetSliderDialogRange(1.0, 10.0)
        SetSliderDialogInterval(1.0)
    ElseIf option == _exprIntensityOID
        SetSliderDialogStartValue(Main.fExpressionIntensity)
        SetSliderDialogDefaultValue(0.5)
        SetSliderDialogRange(0.0, 1.0)
        SetSliderDialogInterval(0.05)
    EndIf
EndEvent

Event OnOptionSliderAccept(Int option, Float value)
    If option == _hugDurOID
        Main.fHugLoopDuration = value
        SetSliderOptionValue(_hugDurOID, value, "{0}s")
    ElseIf option == _molestDurOID
        Main.fMolestLoopDuration = value
        SetSliderOptionValue(_molestDurOID, value, "{0}s")
    ElseIf option == _kissDurOID
        Main.fKissLoopDuration = value
        SetSliderOptionValue(_kissDurOID, value, "{0}s")
    ElseIf option == _touchDurOID
        Main.fTouchLoopDuration = value
        SetSliderOptionValue(_touchDurOID, value, "{0}s")
    ElseIf option == _stageDurOID
        Main.fSequenceStageTimer = value
        SetSliderOptionValue(_stageDurOID, value, "{0}s")
    ElseIf option == _cooldownOID
        Main.fNPCGlobalCooldown = value
        SetSliderOptionValue(_cooldownOID, value, "{0}s")
    ElseIf option == _resistDifficultyOID
        Main.fResistDifficulty = value
        SetSliderOptionValue(_resistDifficultyOID, value, "{0}%")
    ElseIf option == _escalationWindowOID
        Main.fEscalationWindow = value
        SetSliderOptionValue(_escalationWindowOID, value, "{0}s")
    ElseIf option == _escalationDifficultyOID
        Main.fEscalationDifficulty = value
        SetSliderOptionValue(_escalationDifficultyOID, value, "{0}%")
    ElseIf option == _spankTatIntensityOID
        Main.SpankTatIntensity = value as Int
        SetSliderOptionValue(_spankTatIntensityOID, value, "{0}")
    ElseIf option == _spankHealFactorOID
        Main.SpankHealFactor = value as Int
        Main.ApplyFadeSettings()
        SetSliderOptionValue(_spankHealFactorOID, value, "{0} hr")
    ElseIf option == _exprIntensityOID
        Main.fExpressionIntensity = value
        SetSliderOptionValue(_exprIntensityOID, value, "{2}")
    EndIf
EndEvent

Event OnOptionHighlight(Int option)
    If option == _enabledOID
        SetOptionHighlightText("Master switch for all Baka motion interactions.")
    ElseIf option == _animatedTearsOID
        SetOptionHighlightText("Apply animated 3D tears to female victims during aggressive scenes. Requires Emotional Tears Effect SE (EmoTears4NPCs.esp). Has no effect if that mod is not installed.")
    ElseIf option == _playerTargetOID
        SetOptionHighlightText("Allow NPCs to initiate Baka animations on the player.")
    ElseIf option == _femaleOnlyOID
        SetOptionHighlightText("Restrict ALL actions to female targets. Note: breast/intimate actions always require female targets regardless of this setting.")
    ElseIf option == _cooldownOID
        SetOptionHighlightText("Minimum real-time seconds between NPC-initiated actions. Default 30s.")
    ElseIf option == _hugDurOID
        SetOptionHighlightText("How long BackHug and FrontHug loop phases last.")
    ElseIf option == _molestDurOID
        SetOptionHighlightText("How long BackHugMolest loop phase lasts.")
    ElseIf option == _kissDurOID
        SetOptionHighlightText("Time held on each stage of kiss animations.")
    ElseIf option == _touchDurOID
        SetOptionHighlightText("Duration of single-shot touch animations.")
    ElseIf option == _stageDurOID
        SetOptionHighlightText("Seconds per stage in multi-stage sequences (Struggle, ChokeHug, Drunk, etc.).")
    ElseIf option == _resistEnabledOID
        SetOptionHighlightText("When enabled, the Flash Games QTE overlay appears during forced animations. Player can fight back using the configured keys.")
    ElseIf option == _resistDifficultyOID
        SetOptionHighlightText("How easy it is to escape the main QTE. 70 = default. Higher = easier. Lower = harder.")
    ElseIf option == _escalationWindowOID
        SetOptionHighlightText("After a QTE defeat, how many seconds the attacker has to choose to escalate to SexLab. If nothing happens the victim is released.")
    ElseIf option == _escalationDifficultyOID
        SetOptionHighlightText("Difficulty of the second QTE (choke hold) that determines whether SexLab triggers. Same scale as Escape Difficulty.")
    ElseIf option == _spankPlayerOID
        SetOptionHighlightText("Allow the player character to be spanked by NPCs.")
    ElseIf option == _spankMaleOID
        SetOptionHighlightText("Allow male actors to be spanked. Off by default — moans and marks are female-only, so male spank is just the impact sound.")
    ElseIf option == _spankFurnitureOID
        SetOptionHighlightText("Enable special reactions when the target is using furniture (alchemy lab, forge, etc).")
    ElseIf option == _spankTatIntensityOID
        SetOptionHighlightText("How many spanks are needed to advance one mark stage. At 2: 2 spanks = light marks, 4 = medium, 6 = heavy. Men never receive marks.")
    ElseIf option == _spankHealFactorOID
        SetOptionHighlightText("In-game hours for each mark stage to heal away. At 2: full marks heal in about 8 in-game hours.")
    EndIf
EndEvent
