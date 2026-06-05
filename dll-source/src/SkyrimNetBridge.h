#pragma once

#include "SkyrimNet_PublicAPI.h"

// Bridges SNBaka_UI.dll to SkyrimNet's native C++ API (SkyrimNet_PublicAPI.h).
// Resolves the API and registers our action categories. Call Init() once, at
// SKSE kDataLoaded (the API doc requires resolution there; action/category
// registration works immediately after).
namespace SkyrimNetBridge {
    void Init();
}
