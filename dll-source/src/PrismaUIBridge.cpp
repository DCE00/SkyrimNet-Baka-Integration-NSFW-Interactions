#include <SKSE/SKSE.h>
#include "PrismaUIBridge.h"
#include <cstdlib>
#include <format>
#include <RE/V/VirtualMachine.h>
#include <RE/F/FunctionArguments.h>
#include <RE/T/TESDataHandler.h>
#include <RE/T/TESQuest.h>
#include <RE/T/TESObjectCELL.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/C/CrosshairPickData.h>
#include <RE/T/TESFurniture.h>
#include <RE/RTTI.h>
#include <algorithm>

static constexpr const char* kMenuHTMLPath    = "SNBaka_Menu/index.html";
static std::vector<RE::FormID> s_noCollisionActors;
static constexpr const char* kModEventName    = "SNBaka_MenuChoice";
static constexpr float       kSexScanRadius   = 300.0f;  // Skyrim units (~5 m)
static constexpr float       kInteractRadius  = 300.0f;  // fallback nearest-actor range when crosshair is empty

// SexLab.esm raw FormID for SexLabAnimatingFaction (0x0400E50F → local 0xE50F).
// Used as a C++ fallback when the Papyrus IsInSexAnimation faction check races.
static constexpr RE::FormID  kSexLabAnimatingFactionID = 0xE50F;
static constexpr const char* kSexLabESM                = "SexLab.esm";

// Returns true if the player is currently in the SexLabAnimatingFaction.
static bool IsPlayerInSexAnimation() noexcept {
    auto* handler = RE::TESDataHandler::GetSingleton();
    auto* player  = RE::PlayerCharacter::GetSingleton();
    if (!handler || !player) return false;
    auto* faction = handler->LookupForm<RE::TESFaction>(kSexLabAnimatingFactionID, kSexLabESM);
    if (!faction) return false;
    return player->GetFactionRank(faction, true) >= 0;
}

// Returns the actor currently under the player's crosshair, or nullptr.
static RE::Actor* GetCrosshairActor() noexcept {
    auto* pick = RE::CrosshairPickData::GetSingleton();
    if (!pick) return nullptr;
    if (auto ref = pick->targetActor.get()) {
        if (auto* a = skyrim_cast<RE::Actor*>(ref.get()))
            return a;
    }
    if (auto ref = pick->target.get()) {
        if (auto* a = skyrim_cast<RE::Actor*>(ref.get()))
            return a;
    }
    return nullptr;
}

// Fallback when the crosshair is empty: nearest living non-player actor within radius.
static RE::Actor* FindNearestLivingActor(float radius) noexcept {
    auto* player = RE::PlayerCharacter::GetSingleton();
    auto* cell   = player ? player->GetParentCell() : nullptr;
    if (!player || !cell) return nullptr;
    const auto origin = player->GetPosition();
    RE::Actor* best   = nullptr;
    float      bestSq = radius * radius;
    cell->ForEachReferenceInRange(origin, radius,
        [&](RE::TESObjectREFR& refr) -> RE::BSContainer::ForEachResult {
            auto* a = skyrim_cast<RE::Actor*>(&refr);
            if (!a || a->IsPlayerRef() || a->IsDead())
                return RE::BSContainer::ForEachResult::kContinue;
            const auto  p  = a->GetPosition();
            const float dx = p.x - origin.x, dy = p.y - origin.y, dz = p.z - origin.z;
            const float sq = dx * dx + dy * dy + dz * dz;
            if (sq < bestSq) { bestSq = sq; best = a; }
            return RE::BSContainer::ForEachResult::kContinue;
        });
    return best;
}

// Plan A (the proven one): instead of poking havok ourselves — which we could never
// verify against the Steam-DRM-encrypted SkyrimSE.exe — we flag the actor as a PLAYER
// TEAMMATE.  The already-installed DisableFollowerCollision reads exactly this bit
// (BOOL_BITS::kPlayerTeammate = 1<<26 = 0x4000000 at actor+0xE0, via IsPlayerTeammate())
// every frame and disables that actor's collision with the player.  The user proved this
// works by adding an NPC to the follower faction.  Restored when the scene ends.
static void SetActorNoCharCollision(RE::Actor* actor, bool disable, bool a_log = true) noexcept {
    if (!actor) return;
    // COLLISION ABANDONED: the player-teammate flag is intentionally NOT set.  It was the
    // only way to get DFC to turn off NPC↔player collision, but flagging NPCs as teammates
    // drags them into the game's follower/teammate AI processing, which accumulates and
    // slows the game (and any scene that doesn't clean up leaves a stuck teammate).  Not
    // worth the cost — scenes rely on the vehicle pin + SetDontMove + DoNothing hold instead.
    // (Re-enable the two lines below only if you ever want DFC collision-off back.)
    // if (disable) actor->GetActorRuntimeData().boolBits.set(RE::Actor::BOOL_BITS::kPlayerTeammate);
    // else         actor->GetActorRuntimeData().boolBits.reset(RE::Actor::BOOL_BITS::kPlayerTeammate);
    if (a_log) {
        SKSE::log::info("[SNBakaUI]   collision no-op for '{}' (disable={}, teammate flag abandoned)",
            actor->GetDisplayFullName(), disable);
    }
}

// The teammate flag is a persistent actor bit — set it once, DFC reads it every frame,
// and the Papyrus DoNothing package override (added at scene start) keeps the follower
// AI it triggers from breaking the held animation.  No per-frame C++ hook needed.

// Restore collision on all actors we previously modified.
void PrismaUIBridge::RestoreTrackedCollision() noexcept {
    for (auto id : s_noCollisionActors) {
        auto* actor = RE::TESForm::LookupByID<RE::Actor>(id);
        SetActorNoCharCollision(actor, false);
    }
    s_noCollisionActors.clear();
    SKSE::log::info("[SNBakaUI] RestoreTrackedCollision: done.");
}

// Per-actor collision toggle, exposed to Papyrus.  Called for both participants
// at animation start (disable) and from _CleanupPair (restore).  Tracks FormIDs
// so RestoreTrackedCollision() can clear anything left dangling after a load.
void PrismaUIBridge::SetActorCollision(RE::Actor* actor, bool disable) noexcept {
    if (!actor) return;
    // Never flag the PLAYER as their own teammate.  DFC disables the NPC's collision
    // against the player, so flagging just the NPC is enough.
    if (actor->IsPlayerRef()) {
        SKSE::log::info("[SNBakaUI] SetActorCollision: skipping player.");
        return;
    }
    const auto id = actor->GetFormID();
    if (disable) {
        const bool alreadyTracked =
            std::find(s_noCollisionActors.begin(), s_noCollisionActors.end(), id) != s_noCollisionActors.end();
        SetActorNoCharCollision(actor, true, /*a_log=*/!alreadyTracked);
        if (!alreadyTracked) {
            s_noCollisionActors.push_back(id);
            SKSE::log::info("[SNBakaUI] SetActorCollision: '{}' disable=true (tracked={})",
                actor->GetDisplayFullName(), s_noCollisionActors.size());
        }
        return;
    } else {
        SetActorNoCharCollision(actor, false);
        s_noCollisionActors.erase(
            std::remove(s_noCollisionActors.begin(), s_noCollisionActors.end(), id),
            s_noCollisionActors.end());
    }
    SKSE::log::info("[SNBakaUI] SetActorCollision: '{}' disable={} (tracked={})",
        actor->GetDisplayFullName(), disable, s_noCollisionActors.size());
}

bool PrismaUIBridge::IsAlchemyOrEnchantingFurniture(RE::TESObjectREFR* furniture) noexcept {
    if (!furniture) return false;
    auto* base = furniture->GetBaseObject();
    auto* furn = base ? base->As<RE::TESFurniture>() : nullptr;
    if (!furn) return false;
    using BT = RE::TESFurniture::WorkBenchData::BenchType;
    switch (furn->workBenchData.benchType.get()) {
    case BT::kAlchemy:
    case BT::kAlchemyExperiment:
    case BT::kEnchanting:
    case BT::kEnchantingExperiment:
        return true;
    default:
        return false;
    }
}

void PrismaUIBridge::RequestAPI() noexcept {
    s_prisma = static_cast<PRISMA_UI_API::IVPrismaUI1*>(
        PRISMA_UI_API::RequestPluginAPI(PRISMA_UI_API::InterfaceVersion::V1));
    if (s_prisma)
        SKSE::log::info("[SNBakaUI] PrismaUI API acquired.");
    else
        SKSE::log::warn("[SNBakaUI] PrismaUI not found — menus will fall back to vanilla messageboxes.");
}

void PrismaUIBridge::CreateMenuView() noexcept {
    if (!s_prisma) return;
    if (s_view && s_prisma->IsValid(s_view)) return; // already valid
    s_view = s_prisma->CreateView(kMenuHTMLPath);
    if (!s_prisma->IsValid(s_view)) {
        SKSE::log::error("[SNBakaUI] Failed to create SNBaka_Menu view at '{}'.", kMenuHTMLPath);
        return;
    }
    s_prisma->Hide(s_view);
    s_prisma->RegisterJSListener(s_view, "snbaka_chose", OnJSChoice);
    SKSE::log::info("[SNBakaUI] SNBaka_Menu view created and JS listener registered.");
}

bool PrismaUIBridge::IsAvailable() noexcept {
    return s_prisma && s_prisma->IsValid(s_view);
}

bool PrismaUIBridge::IsMenuOpen() noexcept {
    return s_mode.load() != MenuMode::None;
}

void PrismaUIBridge::CancelMenu() noexcept {
    if (!IsMenuOpen()) return;
    // The encounter wizard's cancel signal is the string "cancel"; the other
    // menus use "-1".  Sending the wrong token to _StartSexLabScene would start
    // an unintended default scene instead of aborting.
    if (s_mode.load() == MenuMode::Encounter)
        OnJSChoice("cancel");
    else
        OnJSChoice("-1");
}

RE::Actor* PrismaUIBridge::GetInteractTarget() noexcept {
    auto* player = RE::PlayerCharacter::GetSingleton();
    RE::Actor* t = GetCrosshairActor();          // precise: what you're looking at
    if (!t || t == player)
        t = FindNearestLivingActor(kInteractRadius); // fallback: nearest in range
    if (t == player) t = nullptr;
    SKSE::log::info("[SNBakaUI] GetInteractTarget -> {} (0x{:08X})",
        t ? t->GetDisplayFullName() : "(none)", t ? t->GetFormID() : 0);
    return t;
}

void PrismaUIBridge::ShowInteractMenu(RE::Actor* caster, RE::Actor* target) noexcept {
    if (!IsAvailable()) {
        // View was invalidated (PrismaUI reset after save load) — try to recreate.
        SKSE::log::warn("[SNBakaUI] ShowInteractMenu: view invalid, attempting recovery.");
        CreateMenuView();
        if (!IsAvailable()) {
            SKSE::log::error("[SNBakaUI] ShowInteractMenu: recovery failed, aborting.");
            return;
        }
    }

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!caster) caster = player;

    // Sex check FIRST — works regardless of crosshair/target, so it's reliable
    // even though the spell is self-delivered (akTarget == player).
    if (IsPlayerInSexAnimation()) {
        SKSE::log::info("[SNBakaUI] ShowInteractMenu: in sex animation -> spank menu.");
        ShowSexSpankMenu("{\"names\":[],\"playerInScene\":true}");
        return;
    }

    // Self-delivered spell hands us the player as 'target'.  Resolve the real
    // interact target from the crosshair, with a nearest-actor fallback.
    if (!target || target == player) {
        RE::Actor* resolved = GetCrosshairActor();
        if (!resolved || resolved == player)
            resolved = FindNearestLivingActor(kInteractRadius);
        if (resolved && resolved != player) {
            target = resolved;
        } else {
            SKSE::log::warn("[SNBakaUI] ShowInteractMenu: no target in crosshair/range — nothing to interact with.");
            return;
        }
    }

    if (!caster || !target) {
        SKSE::log::error("[SNBakaUI] ShowInteractMenu: null caster or target after resolve.");
        return;
    }

    // Capture the actors now, by FormID, so the dispatch uses these exact actors
    // regardless of what happens to Papyrus _pending* while the menu is open.
    s_interactCaster = caster->GetFormID();
    s_interactTarget = target->GetFormID();
    SKSE::log::info("[SNBakaUI] ShowInteractMenu: caster='{}' (0x{:08X})  target='{}' (0x{:08X})",
        caster->GetDisplayFullName(), s_interactCaster,
        target->GetDisplayFullName(), s_interactTarget);

    std::string safe = target->GetDisplayFullName();
    for (auto& c : safe) if (c == '\'') c = '\x60';
    s_mode = MenuMode::Interact;
    const auto script = std::format("window.snbaka_open_interact('{}')", safe);
    s_prisma->Show(s_view);
    s_prisma->Invoke(s_view, script.c_str());
    // Pause while choosing so the target NPC can't wander off / combat can't move
    // things during the few seconds of selection.  Cursor still works with the
    // focus menu registered (disableFocusMenu=false).
    s_prisma->Focus(s_view, /*pauseGame=*/true, /*disableFocusMenu=*/false);
}

void PrismaUIBridge::ShowEncounterMenu(RE::Actor* aggressor, RE::Actor* victim) noexcept {
    if (!IsAvailable()) {
        CreateMenuView();
        if (!IsAvailable()) {
            SKSE::log::error("[SNBakaUI] ShowEncounterMenu: view unavailable.");
            return;
        }
    }
    if (!aggressor || !victim) {
        SKSE::log::error("[SNBakaUI] ShowEncounterMenu: null aggressor/victim.");
        return;
    }

    s_encAggressor = aggressor->GetFormID();
    s_encVictim    = victim->GetFormID();
    SKSE::log::info("[SNBakaUI] ShowEncounterMenu: aggressor='{}' (0x{:08X}) victim='{}' (0x{:08X})",
        aggressor->GetDisplayFullName(), s_encAggressor,
        victim->GetDisplayFullName(), s_encVictim);

    // Build {"aggressor":"..","victim":".."} for the wizard header (display only).
    std::string agg = aggressor->GetDisplayFullName();
    std::string vic = victim->GetDisplayFullName();
    for (auto& c : agg) if (c == '\'') c = '\x60';
    for (auto& c : vic) if (c == '\'') c = '\x60';
    const auto json   = std::format("{{\"aggressor\":\"{}\",\"victim\":\"{}\"}}", agg, vic);
    const auto script = std::format("window.snbaka_open_encounter('{}')", json);

    s_mode = MenuMode::Encounter;
    s_prisma->Show(s_view);
    s_prisma->Invoke(s_view, script.c_str());
    // Pause during the multi-step wizard (no active SexLab scene yet — StartSex
    // runs only after the picks), so nothing drifts during the 3-4 clicks.
    s_prisma->Focus(s_view, /*pauseGame=*/true, /*disableFocusMenu=*/false);
}

void PrismaUIBridge::ShowSexSpankMenu(const std::string& json) noexcept {
    if (!IsAvailable()) {
        CreateMenuView();
        if (!IsAvailable()) {
            SKSE::log::error("[SNBakaUI] ShowSexSpankMenu: view unavailable.");
            return;
        }
    }

    SKSE::log::info("[SNBakaUI] ShowSexSpankMenu: papyrus json='{}'", json);

    // Reset C++ actor state for this menu open.
    s_usingCppSexActors = false;
    s_sexActorCount     = 0;
    s_sexActorIds.fill(0);

    // Detect whether Papyrus found any NPCs: names array is non-empty if it
    // contains at least one quoted name, i.e. `"names":["`.
    const bool papyrusHasActors = json.find("\"names\":[\"") != std::string::npos;
    SKSE::log::info("[SNBakaUI] ShowSexSpankMenu: papyrusHasActors={}", papyrusHasActors);

    std::string effectiveJson = json;

    if (!papyrusHasActors) {
        // Papyrus-side detection (faction-based) found no actors.  Fall back to
        // a proximity scan — during a sex animation participants are always within
        // a few metres of the player.
        SKSE::log::info("[SNBakaUI] ShowSexSpankMenu: no Papyrus actors — proximity scan (r={})", kSexScanRadius);

        auto* player = RE::PlayerCharacter::GetSingleton();
        auto* cell   = player ? player->GetParentCell() : nullptr;

        if (player && cell) {
            const auto origin = player->GetPosition();
            cell->ForEachReferenceInRange(origin, kSexScanRadius,
                [&](RE::TESObjectREFR& refr) -> RE::BSContainer::ForEachResult {
                    if (s_sexActorCount >= 3)
                        return RE::BSContainer::ForEachResult::kStop;
                    auto* actor = skyrim_cast<RE::Actor*>(&refr);
                    if (!actor || actor->IsPlayerRef() || actor->IsDead())
                        return RE::BSContainer::ForEachResult::kContinue;
                    s_sexActorIds[s_sexActorCount] = actor->GetFormID();
                    SKSE::log::info("[SNBakaUI]   scanned[{}] = '{}' (0x{:08X})",
                        s_sexActorCount,
                        actor->GetDisplayFullName(),
                        actor->GetFormID());
                    ++s_sexActorCount;
                    return RE::BSContainer::ForEachResult::kContinue;
                });
        }

        if (s_sexActorCount > 0) {
            s_usingCppSexActors = true;

            // Rebuild JSON from scanned actors.
            const bool playerInScene = json.find("\"playerInScene\":true") != std::string::npos;
            std::string names;
            for (std::uint8_t i = 0; i < s_sexActorCount; ++i) {
                auto* a = RE::TESForm::LookupByID<RE::Actor>(s_sexActorIds[i]);
                if (i > 0) names += ",";
                names += "\"";
                names += a ? a->GetDisplayFullName() : "???";
                names += "\"";
            }
            effectiveJson = std::format(
                "{{\"names\":[{}],\"playerInScene\":{}}}",
                names, playerInScene ? "true" : "false");
            SKSE::log::info("[SNBakaUI] ShowSexSpankMenu: rebuilt json='{}'", effectiveJson);
        } else {
            SKSE::log::warn("[SNBakaUI] ShowSexSpankMenu: no actors within {}u — menu will be empty", kSexScanRadius);
        }
    }

    s_mode = MenuMode::SexSpank;
    // Escape single-quotes in NPC names before embedding into the JS call.
    for (auto& c : effectiveJson) if (c == '\'') c = '\x60';
    const auto script = std::format("window.snbaka_open_sexspank('{}')", effectiveJson);
    s_prisma->Show(s_view);
    s_prisma->Invoke(s_view, script.c_str());
    s_prisma->Focus(s_view, /*pauseGame=*/false, /*disableFocusMenu=*/false);
}

void PrismaUIBridge::OnJSChoice(const char* value) noexcept {
    if (!s_prisma) return;

    SKSE::log::info("[SNBakaUI] OnJSChoice: value='{}'", value ? value : "(null)");

    s_prisma->Unfocus(s_view);
    s_prisma->Hide(s_view);

    const MenuMode mode = s_mode.exchange(MenuMode::None);
    const int      choice    = value ? std::atoi(value) : -1;
    const float    numArg    = static_cast<float>(choice);
    const char*    strArg    = (mode == MenuMode::SexSpank) ? "sexspank" : "interact";

    // Snapshot C++ actor state so the lambda doesn't race with a future ShowSexSpankMenu.
    const bool                      useCpp  = s_usingCppSexActors;
    const std::array<RE::FormID, 3> actIds  = s_sexActorIds;
    const std::uint8_t              actCnt  = s_sexActorCount;
    const RE::FormID                iCaster = s_interactCaster;
    const RE::FormID                iTarget = s_interactTarget;
    const RE::FormID                eAgg    = s_encAggressor;
    const RE::FormID                eVic    = s_encVictim;
    const std::string               spec    = value ? value : "";   // encounter: "role;intensity;flavor;type" or "cancel"

    SKSE::log::info("[SNBakaUI] OnJSChoice: mode={} choice={} useCpp={} spec='{}'",
        strArg, choice, useCpp, spec);

    SKSE::GetTaskInterface()->AddTask([numArg, strArg, mode, choice, useCpp, actIds, actCnt, iCaster, iTarget, eAgg, eVic, spec]() {
        auto* vm      = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        auto* handler = RE::TESDataHandler::GetSingleton();
        if (!vm || !handler) {
            SKSE::log::error("[SNBakaUI] Task: VM or DataHandler unavailable.");
            return;
        }

        auto* quest = handler->LookupForm<RE::TESQuest>(0x000D62, "SkyrimNet_BakaIntegration.esp");
        if (!quest) {
            SKSE::log::error("[SNBakaUI] Task: BakaIntegration quest not found.");
            return;
        }

        auto* policy = vm->GetObjectHandlePolicy();
        RE::VMHandle handle = policy->GetHandleForObject(RE::FormType::Quest, quest);
        if (handle == policy->EmptyHandle()) {
            SKSE::log::error("[SNBakaUI] Task: VMHandle invalid.");
            return;
        }

        // Collision is now disabled per-pair from Papyrus (SetNoCollision) inside
        // the paired-animation functions, so it covers NPC-NPC too and restores
        // cleanly in _CleanupPair.

        // ── Encounter: split spec "role;intensity;flavor;type" → 4 strings ──────
        if (mode == MenuMode::Encounter) {
            auto* agg = RE::TESForm::LookupByID<RE::Actor>(eAgg);
            auto* vic = RE::TESForm::LookupByID<RE::Actor>(eVic);
            SKSE::log::info("[SNBakaUI] Task: encounter spec='{}' aggressor='{}' victim='{}'",
                spec,
                agg ? agg->GetDisplayFullName() : "(null)",
                vic ? vic->GetDisplayFullName() : "(null)");
            if (!agg || !vic) {
                SKSE::log::error("[SNBakaUI] Task: encounter — actor lookup failed.");
                return;
            }
            // Default role "cancel" so an empty/short spec is treated as a cancel.
            std::string parts[4] = { "cancel", "", "", "" };
            {
                std::size_t start = 0;
                int idx = 0;
                while (idx < 4) {
                    const std::size_t sep = spec.find(';', start);
                    parts[idx++] = (sep == std::string::npos) ? spec.substr(start)
                                                              : spec.substr(start, sep - start);
                    if (sep == std::string::npos) break;
                    start = sep + 1;
                }
            }
            auto* args = RE::MakeFunctionArguments(
                RE::BSFixedString(parts[0].c_str()),
                RE::BSFixedString(parts[1].c_str()),
                RE::BSFixedString(parts[2].c_str()),
                RE::BSFixedString(parts[3].c_str()),
                static_cast<RE::Actor*>(agg),
                static_cast<RE::Actor*>(vic));
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> cb;
            vm->DispatchMethodCall(handle,
                RE::BSFixedString("SkyrimNet_BakaIntegration"),
                RE::BSFixedString("_StartSexLabScene"),
                args, cb);
            delete args;
            SKSE::log::info("[SNBakaUI] Task: _StartSexLabScene dispatched (role='{}').", parts[0]);
            return;
        }

        // ── Interact: dispatch with the actors captured at menu-open ────────────
        // Bypasses Papyrus _pendingTarget/_pendingCaster entirely so the open
        // (unpaused) menu can't leave us with stale/clobbered actors.
        if (mode == MenuMode::Interact) {
            if (choice < 0) {
                SKSE::log::info("[SNBakaUI] Task: interact cancelled.");
                return;
            }
            auto* cst = RE::TESForm::LookupByID<RE::Actor>(iCaster);
            auto* tgt = RE::TESForm::LookupByID<RE::Actor>(iTarget);
            SKSE::log::info("[SNBakaUI] Task: interact dispatch — caster='{}' (0x{:08X}) target='{}' (0x{:08X}) choice={}",
                cst ? cst->GetDisplayFullName() : "(null)", iCaster,
                tgt ? tgt->GetDisplayFullName() : "(null)", iTarget, choice);
            if (!cst || !tgt) {
                SKSE::log::error("[SNBakaUI] Task: interact — actor lookup failed.");
                return;
            }
            auto* args = RE::MakeFunctionArguments(
                std::int32_t{choice},
                static_cast<RE::Actor*>(cst),
                static_cast<RE::Actor*>(tgt));
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> cb;
            vm->DispatchMethodCall(handle,
                RE::BSFixedString("SkyrimNet_BakaIntegration"),
                RE::BSFixedString("_DispatchInteractActionWithActors"),
                args, cb);
            delete args;
            SKSE::log::info("[SNBakaUI] Task: _DispatchInteractActionWithActors dispatched.");
            return;
        }

        // ── SexSpank with C++ scanned actors ────────────────────────────────────
        if (mode == MenuMode::SexSpank && useCpp) {
            if (choice < 0) {
                SKSE::log::info("[SNBakaUI] Task: SexSpank CPP — cancelled.");
                return;
            }

            auto* player = RE::PlayerCharacter::GetSingleton();
            RE::Actor* spanker = nullptr;
            RE::Actor* spankee = nullptr;

            if (choice <= 2) {
                // Player spanks scanned NPC[choice]
                spanker = player;
                if (choice < actCnt)
                    spankee = RE::TESForm::LookupByID<RE::Actor>(actIds[choice]);
            } else if (choice >= 10 && choice <= 12) {
                // Scanned NPC[choice-10] spanks player
                const int idx = choice - 10;
                if (idx < actCnt)
                    spanker = RE::TESForm::LookupByID<RE::Actor>(actIds[idx]);
                spankee = player;
            } else if (choice == 13) {
                spanker = spankee = player;
            }

            SKSE::log::info("[SNBakaUI] Task: SexSpank CPP — spanker='{}' spankee='{}'",
                spanker ? spanker->GetDisplayFullName() : "(null)",
                spankee ? spankee->GetDisplayFullName() : "(null)");

            if (!spanker || !spankee) {
                SKSE::log::warn("[SNBakaUI] Task: SexSpank CPP — missing actor for choice {}", choice);
                return;
            }

            auto* args = RE::MakeFunctionArguments(
                static_cast<RE::Actor*>(spanker),
                static_cast<RE::Actor*>(spankee));
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> cb;
            vm->DispatchMethodCall(handle,
                RE::BSFixedString("SkyrimNet_BakaIntegration"),
                RE::BSFixedString("_SexSpank_Execute"),
                args, cb);
            delete args;
            SKSE::log::info("[SNBakaUI] Task: _SexSpank_Execute dispatched.");
            return;
        }

        // ── Normal path: interact or SexSpank with Papyrus-detected actors ──────
        SKSE::log::info("[SNBakaUI] Task: dispatching OnSNBakaMenuChoice — strArg='{}' numArg={}", strArg, numArg);
        auto* args = RE::MakeFunctionArguments(
            RE::BSFixedString(kModEventName),
            RE::BSFixedString(strArg),
            float{numArg},
            static_cast<RE::TESForm*>(nullptr));
        vm->SendEvent(handle, RE::BSFixedString("OnSNBakaMenuChoice"), args);
        delete args;
        SKSE::log::info("[SNBakaUI] Task: SendEvent complete.");
    });
}
