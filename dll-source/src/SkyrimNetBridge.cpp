#include "PCH.h"
#include "SkyrimNetBridge.h"

// IMPORTANT: SkyrimNet_PublicAPI.h DEFINES the Public* function-pointer globals at file
// scope, so it must be included in EXACTLY ONE translation unit. This is that unit — do
// not include it anywhere else (it would cause LNK2005 multiple-definition errors).
#include "SkyrimNet_PublicAPI.h"

namespace {
    // --- Sex-scene factions (resolved lazily; null when the framework isn't installed) ---
    // Mirrors the Papyrus gating: SexLabAnimatingFaction (SexLab.esm 0xE50F) and
    // OStimActorCountFaction (OStim.esp 0xECA) = "currently in an active sex scene".
    RE::TESFaction* g_sexlabAnimating  = nullptr;
    RE::TESFaction* g_ostimActorCount  = nullptr;
    bool            g_factionsResolved = false;

    void ResolveFactions() {
        if (g_factionsResolved) {
            return;
        }
        g_factionsResolved = true;
        if (auto* dh = RE::TESDataHandler::GetSingleton()) {
            g_sexlabAnimating = dh->LookupForm<RE::TESFaction>(0x00E50F, "SexLab.esm");
            g_ostimActorCount = dh->LookupForm<RE::TESFaction>(0x000ECA, "OStim.esp");
        }
    }

    bool InSexScene(RE::Actor* a) {
        ResolveFactions();
        if (g_sexlabAnimating && a->IsInFaction(g_sexlabAnimating)) {
            return true;
        }
        if (g_ostimActorCount && a->IsInFaction(g_ostimActorCount)) {
            return true;
        }
        return false;
    }

    // Eligibility predicates (visibility gates). NOTE: SkyrimNet may invoke these off the
    // main thread, so they only do cheap, read-only actor queries.
    bool NotInCombat(RE::Actor* a)   { return a && !a->IsInCombat(); }
    bool PreSexContact(RE::Actor* a) { return a && !a->IsInCombat() && !InSexScene(a); }

    // Register one top-level category (customParentCategory = "" => top level). The YAML
    // actions nest into it by setting `customCategory: <id>` to the same id string.
    void RegisterOne(const char* id, const char* description, std::function<bool(RE::Actor*)> elig) {
        const bool ok = PublicRegisterCPPSubCategory(
            id,                 // name (unique identifier)
            description,        // shown to the LLM at category-selection time
            std::move(elig),    // visibility gate
            "",                 // triggeringEventTypesCSV
            50,                 // priority
            "{}",               // parameterSchemaJSON (reserved)
            id,                 // customCategory (what YAML actions reference)
            "",                 // customParentCategory ("" = top level)
            "");                // tagsCSV
        SKSE::log::info("[SNBaka] RegisterSubCategory '{}' -> {}", id, ok);
    }

    void RegisterCategories() {
        RegisterOne("SNBaka_Spank",
            "Open-handed strikes and blows for impact, punishment, or dominance: spank or smack the butt, "
            "slap the face/cheek, slap or strike the breasts/chest, strike the belly. Bare-hand impact, not weapon combat.",
            [](RE::Actor* a) { return NotInCombat(a); });

        RegisterOne("SNBaka_Grab",
            "Seizing, restraining, choking, and subduing BEFORE any sex scene: hug/embrace from front or behind, "
            "grab or grope from behind against their will, choke/strangle/throttle by the throat, wrestle/struggle/pin, "
            "spike a drink or drug their food, exploit a drunk or incapacitated target.",
            [](RE::Actor* a) { return PreSexContact(a); });

        RegisterOne("SNBaka_Intimate",
            "Sexual touching, kissing, and oral contact short of a full sex scene: kiss (forced or loving), "
            "touch/grope/fondle breasts, suck breasts, touch or play with the genitals/privates, examine the intimate "
            "lower body, kneel and perform oral.",
            [](RE::Actor* a) { return PreSexContact(a); });

        RegisterOne("SNBaka_Display",
            "Provocative, performative, social display with NO physical contact: flirt, tease, seduce, make eyes; "
            "or show off, pose, strut, flaunt the body. Use when being suggestive rather than touching anyone.",
            [](RE::Actor* a) { return NotInCombat(a); });

        RegisterOne("SNBaka_Scene",
            "Manage or control a capture/defeat encounter: escalate on a downed victim, release/free the victim, "
            "call off or abort, interrupt/stop an active scene, inspect a captive or investigate a restrained target.",
            [](RE::Actor* a) { return NotInCombat(a); });

        RegisterOne("SNBaka_Expression",
            "Make the speaker's FACE show an emotion for a few seconds (facial expression only, no body action): "
            "happy, angry, afraid, sad, pained, surprised, or confused. Use to react emotionally to what was said or done.",
            [](RE::Actor* a) { return NotInCombat(a); });
    }
}

void SkyrimNetBridge::Init() {
    if (!FindFunctions()) {
        SKSE::log::warn("[SNBaka] SkyrimNet.dll not found - action categories not registered.");
        return;
    }
    const int ver = PublicGetVersion ? PublicGetVersion() : 0;
    SKSE::log::info("[SNBaka] SkyrimNet C++ API v{} detected.", ver);

    if (!PublicRegisterCPPSubCategory) {
        SKSE::log::warn("[SNBaka] PublicRegisterCPPSubCategory unavailable (needs SkyrimNet API v2+); skipping categories.");
        return;
    }
    RegisterCategories();
    SKSE::log::info("[SNBaka] Category registration complete.");
}
