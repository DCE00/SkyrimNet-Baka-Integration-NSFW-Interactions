# SKSEPlugin.cmake - Setup for SKSE plugin using CommonLibSSE-NG submodule

# CommonLibSSE-NG configuration
set(CommonLibPath "external/commonlibsse-ng")
set(CommonLibName "CommonLibSSE")

# Extract version from CommonLibSSE's CMakeLists.txt (project() VERSION doesn't propagate to parent scope)
file(READ "${CMAKE_SOURCE_DIR}/${CommonLibPath}/CMakeLists.txt" _commonlib_cmakelists_content)
string(REGEX MATCH "VERSION[ \t]+([0-9]+\\.[0-9]+\\.[0-9]+)" _ "${_commonlib_cmakelists_content}")
set(COMMONLIBSSE_VERSION "${CMAKE_MATCH_1}" CACHE STRING "CommonLibSSE-NG version" FORCE)
message(STATUS "Configuring CommonLibSSE-NG version ${COMMONLIBSSE_VERSION}")

# Save original build type
set(_saved_build_type "${CMAKE_BUILD_TYPE}")
set(_saved_cmake_message_log_level "${CMAKE_MESSAGE_LOG_LEVEL}")

set(CMAKE_MESSAGE_LOG_LEVEL INFO)

# Always build CommonLibSSE in Release mode to disable assertions and enable optimizations
set(CMAKE_BUILD_TYPE "Release")
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_definitions(NDEBUG)

# Disable CommonLibSSE tests when building as subdirectory
set(BUILD_TESTS OFF CACHE BOOL "Disable CommonLibSSE tests" FORCE)

# Add CommonLibSSE-NG as a subdirectory with EXCLUDE_FROM_ALL
add_subdirectory("${CommonLibPath}" "${BUILD_ROOT}/external_builds/${CommonLibName}" EXCLUDE_FROM_ALL)

# Include the CommonLibSSE helper cmake functions (provides add_commonlibsse_plugin macro)
include("${CommonLibPath}/cmake/CommonLibSSE.cmake")

# Restore original build type for the main project
set(CMAKE_BUILD_TYPE "${_saved_build_type}" CACHE STRING "" FORCE)
if(DEFINED _saved_cmake_message_log_level AND NOT "${_saved_cmake_message_log_level}" STREQUAL "")
    set(CMAKE_MESSAGE_LOG_LEVEL "${_saved_cmake_message_log_level}")
    set(CMAKE_MESSAGE_LOG_LEVEL "${_saved_cmake_message_log_level}" CACHE STRING "" FORCE)
else()
    unset(CMAKE_MESSAGE_LOG_LEVEL)
endif()

# Expose CommonLibSSE version to C++ code
add_compile_definitions(COMMONLIBSSE_VERSION="${COMMONLIBSSE_VERSION}")
