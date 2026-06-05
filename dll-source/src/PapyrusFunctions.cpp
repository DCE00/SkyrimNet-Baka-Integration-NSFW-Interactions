#include "PapyrusFunctions.h"
#include "PrismaUIBridge.h"

static constexpr const char* kScriptName = "SNBakaUI";

static bool SNBakaUI_IsAvailable(RE::StaticFunctionTag*) {
    const bool result = PrismaUIBridge::IsAvailable();
    if (!result)
        SKSE::log::warn("IsAvailable() = false — view invalid, will try to recover.");
    return result;
}

static void SNBakaUI_ShowInteractMenu(RE::StaticFunctionTag*,
                                      RE::Actor* akCaster, RE::Actor* akTarget) {
    PrismaUIBridge::ShowInteractMenu(akCaster, akTarget);
}

static RE::Actor* SNBakaUI_GetInteractTarget(RE::StaticFunctionTag*) {
    return PrismaUIBridge::GetInteractTarget();
}

static void SNBakaUI_SetNoCollision(RE::StaticFunctionTag*, RE::Actor* akActor, bool abDisable) {
    PrismaUIBridge::SetActorCollision(akActor, abDisable);
}

static bool SNBakaUI_IsCraftingTemptation(RE::StaticFunctionTag*, RE::TESObjectREFR* akFurniture) {
    return PrismaUIBridge::IsAlchemyOrEnchantingFurniture(akFurniture);
}

static void SNBakaUI_ShowSexSpankMenu(RE::StaticFunctionTag*,
                                      RE::BSFixedString json) {
    SKSE::log::info("ShowSexSpankMenu native called.");
    PrismaUIBridge::ShowSexSpankMenu(json.c_str() ? json.c_str() : "{}");
}

static void SNBakaUI_ShowEncounterMenu(RE::StaticFunctionTag*,
                                       RE::Actor* akAggressor, RE::Actor* akVictim) {
    PrismaUIBridge::ShowEncounterMenu(akAggressor, akVictim);
}

bool PapyrusFunctions::Register(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("IsAvailable",       kScriptName, SNBakaUI_IsAvailable);
    vm->RegisterFunction("ShowInteractMenu",  kScriptName, SNBakaUI_ShowInteractMenu);
    vm->RegisterFunction("ShowSexSpankMenu",  kScriptName, SNBakaUI_ShowSexSpankMenu);
    vm->RegisterFunction("GetInteractTarget",   kScriptName, SNBakaUI_GetInteractTarget);
    vm->RegisterFunction("SetNoCollision",      kScriptName, SNBakaUI_SetNoCollision);
    vm->RegisterFunction("IsCraftingTemptation", kScriptName, SNBakaUI_IsCraftingTemptation);
    vm->RegisterFunction("ShowEncounterMenu",   kScriptName, SNBakaUI_ShowEncounterMenu);
    SKSE::log::info("Papyrus functions registered.");
    return true;
}
