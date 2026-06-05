cmake_minimum_required(VERSION 3.21)
add_compile_definitions(_WIN32_WINNT=0x0A00)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(
    SkyrimNet_BakaIntegration
    VERSION 1.5.1
    LANGUAGES CXX
)

set(PROJECT_OUTPUT_NAME "${PROJECT_NAME}-v${PROJECT_VERSION}")
if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_OUTPUT_NAME}")
else()
    set(OUTPUT_FOLDER "${CMAKE_SOURCE_DIR}/../dist/${PROJECT_OUTPUT_NAME}")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(BUILD_ROOT "${CMAKE_SOURCE_DIR}/build")
set(FETCHCONTENT_BASE_DIR "${BUILD_ROOT}/external_builds")

include(CompilerFlags)
include(commonlibsse)

add_commonlibsse_plugin(${PROJECT_NAME}
    SOURCES
    src/Plugin.cpp
    src/PrismaUIBridge.cpp
    src/PapyrusFunctions.cpp
    src/SkyrimNetBridge.cpp
    NAME ${PROJECT_NAME}
    AUTHOR "Dag"
    EMAIL ""
    USE_ADDRESS_LIBRARY
    MINIMUM_SKSE_VERSION "2.0.1.04"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)
target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE) # <--- enable all spdlog log levels (including trace)
enable_lto_for_target(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE CommonLibSSE::CommonLibSSE)


if(DEFINED OUTPUT_FOLDER)
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")
    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM
    )
else()
    message(WARNING
        "SKYRIM_MODS_FOLDER is not set — the built DLL will NOT be auto-copied. "
        "Set the SKYRIM_MODS_FOLDER environment variable to your mod manager's mods folder "
        "(the parent of the SkyrimNet_BakaIntegration mod), or copy the built DLL manually to "
        "<your SkyrimNet_BakaIntegration mod>/SKSE/Plugins/SkyrimNet_BakaIntegration.dll")
endif()
