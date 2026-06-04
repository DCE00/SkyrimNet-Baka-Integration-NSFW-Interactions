#include "PCH.h"
#include "PrismaUIBridge.h"
#include "PapyrusFunctions.h"
#include "SkyrimNetBridge.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <RE/B/BSInputDeviceManager.h>
#include <RE/I/InputEvent.h>
#include <RE/B/ButtonEvent.h>

// Closes the active PrismaUI menu on ESC or Tab so the player isn't stuck.
class MenuInputHandler : public RE::BSTEventSink<RE::InputEvent*> {
public:
    static MenuInputHandler* GetSingleton() {
        static MenuInputHandler instance;
        return &instance;
    }

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event,
                                          RE::BSTEventSource<RE::InputEvent*>*) override {
        if (!a_event || !PrismaUIBridge::IsMenuOpen())
            return RE::BSEventNotifyControl::kContinue;

        for (auto* ev = *a_event; ev; ev = ev->next) {
            if (ev->GetEventType() != RE::INPUT_EVENT_TYPE::kButton) continue;
            if (ev->GetDevice()    != RE::INPUT_DEVICE::kKeyboard)   continue;
            auto* btn = ev->AsButtonEvent();
            if (!btn || !btn->IsDown()) continue;

            constexpr std::uint32_t kEscape = 1;
            constexpr std::uint32_t kTab    = 15;
            if (btn->idCode == kEscape || btn->idCode == kTab) {
                PrismaUIBridge::CancelMenu();
                return RE::BSEventNotifyControl::kStop;
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }
};

static void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) return;
    auto logPath = *logsFolder / "SNBaka_UI.log";
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath.string(), true);
    auto logger = std::make_shared<spdlog::logger>("SNBaka_UI", std::move(sink));
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
    spdlog::set_default_logger(std::move(logger));
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SetupLog();
    SKSE::log::info("[SNBakaUI] Plugin loaded.");

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* msg) {
        switch (msg->type) {
        case SKSE::MessagingInterface::kPostLoad:
            PrismaUIBridge::RequestAPI();
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            // View created exactly once here — never re-created on save load,
            // matching the known-good behaviour.  ShowInteractMenu self-heals
            // lazily if PrismaUI ever reports the view invalid.
            PrismaUIBridge::CreateMenuView();
            RE::BSInputDeviceManager::GetSingleton()->AddEventSink(MenuInputHandler::GetSingleton());
            SKSE::log::info("[SNBakaUI] Input handler registered.");
            // Resolve SkyrimNet's C++ API and register our action categories.
            SkyrimNetBridge::Init();
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
        case SKSE::MessagingInterface::kNewGame:
            // Clear any no-collision flags left dangling from a prior session.
            // No-op on a fresh process (tracking list is empty).
            PrismaUIBridge::RestoreTrackedCollision();
            break;
        }
    });

    SKSE::GetPapyrusInterface()->Register(PapyrusFunctions::Register);
    return true;
}
