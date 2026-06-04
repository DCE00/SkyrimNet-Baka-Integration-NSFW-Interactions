cmake_minimum_required(VERSION 3.21)

project(
    SNBaka_UI
    VERSION 1.0.0
    LANGUAGES CXX
)

if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/SkyrimNet_BakaIntegration")
endif()

find_package(CommonLibSSE CONFIG REQUIRED)

add_commonlibsse_plugin(${PROJECT_NAME}
    SOURCES
        src/Plugin.cpp
        src/PrismaUIBridge.cpp
        src/PapyrusFunctions.cpp
        src/SkyrimNetBridge.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

# DYNAMIC CRT (/MD) + STATIC libraries.  SkyrimNet's C++ API (SkyrimNet_PublicAPI.h) passes
# std::string / std::function across the DLL boundary, which REQUIRES both DLLs share the
# dynamic CRT (/MD).  But we must NOT depend on shared spdlog.dll/fmt.dll (the plain
# x64-windows triplet builds those as DLLs -> "couldn't load plugin (Error 126)" because
# they aren't shipped).  The x64-windows-static-md vcpkg triplet (see CMakePresets.json)
# gives static libs (spdlog/fmt/CommonLibSSE linked in) WITH the dynamic CRT -> best of both.
# Pin the target runtime to /MD to match that triplet.
set_property(TARGET ${PROJECT_NAME} PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(${PROJECT_NAME} PRIVATE include src)

if(DEFINED OUTPUT_FOLDER)
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")
    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DLL_FOLDER}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM
    )
else()
    message(WARNING
        "SKYRIM_MODS_FOLDER is not set — the built DLL will NOT be auto-copied. "
        "Set the SKYRIM_MODS_FOLDER environment variable to your mod manager's mods folder "
        "(the parent of the SkyrimNet_BakaIntegration mod), or copy the built DLL manually to "
        "<your SkyrimNet_BakaIntegration mod>/SKSE/Plugins/SNBaka_UI.dll")
endif()
