cmake_minimum_required(VERSION 3.24)

# Detect a vcpkg installation automatically if the user
# exports VCPKG_ROOT in their environment before configuring.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE
            "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "vcpkg toolchain file")
    else()
        message(FATAL_ERROR
            "This project uses vcpkg in manifest mode. "
            "Set the VCPKG_ROOT environment variable or pass "
            "-DCMAKE_TOOLCHAIN_FILE=<path to vcpkg.cmake> when configuring.")
    endif()
endif()

project(ZOKATA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build." FORCE)
endif()

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

# vcpkg's glfw port exports a target named 'glfw'.
set(GLFW_IMPORTED_TARGET glfw)
if(NOT TARGET ${GLFW_IMPORTED_TARGET})
    if(TARGET glfw3)
        set(GLFW_IMPORTED_TARGET glfw3)
    elseif(TARGET glfw::glfw)
        set(GLFW_IMPORTED_TARGET glfw::glfw)
    else()
        message(FATAL_ERROR
            "No suitable GLFW target found after find_package(glfw3).")
    endif()
endif()

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ZMATH_HEADERS CONFIGURE_DEPENDS
    "${ROOT_DIR}/include/ZokataMath/*.h"
    "${ROOT_DIR}/include/ZokataMath/*.hpp"
)
file(GLOB_RECURSE ZMATH_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/ZokataMath/*.cpp"
    "${ROOT_DIR}/src/ZokataMath/*.cc"
    "${ROOT_DIR}/src/ZokataMath/*.cxx"
)

add_library(Zokata-math STATIC
    ${ZMATH_SOURCES}
    ${ZMATH_HEADERS}
)

if(ZMATH_HEADERS)
    source_group(TREE "${ROOT_DIR}/include"
        PREFIX "include"
        FILES ${ZMATH_HEADERS})
endif()
if(ZMATH_SOURCES)
    source_group(TREE "${ROOT_DIR}/src"
        PREFIX "src"
        FILES ${ZMATH_SOURCES})
endif()

target_include_directories(Zokata-math
    PUBLIC
        ${ROOT_DIR}/include
    PRIVATE
        ${ROOT_DIR}/src
)

target_link_libraries(Zokata-math
    PUBLIC
        glm::glm
)

target_compile_features(Zokata-math PUBLIC cxx_std_23)

file(GLOB_RECURSE ZLOG_HEADERS CONFIGURE_DEPENDS
    "${ROOT_DIR}/include/ZokataLog/*.h"
    "${ROOT_DIR}/include/ZokataLog/*.hpp"
)
file(GLOB_RECURSE ZLOG_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/ZokataLog/*.cpp"
    "${ROOT_DIR}/src/ZokataLog/*.cc"
    "${ROOT_DIR}/src/ZokataLog/*.cxx"
)

add_library(Zokata-log STATIC
    ${ZLOG_SOURCES}
    ${ZLOG_HEADERS}
)

if(ZLOG_HEADERS)
    source_group(TREE "${ROOT_DIR}/include"
        PREFIX "include"
        FILES ${ZLOG_HEADERS})
endif()
if(ZLOG_SOURCES)
    source_group(TREE "${ROOT_DIR}/src"
        PREFIX "src"
        FILES ${ZLOG_SOURCES})
endif()

target_include_directories(Zokata-log
    PUBLIC
        ${ROOT_DIR}/include
    PRIVATE
        ${ROOT_DIR}/src
)

target_compile_features(Zokata-log PUBLIC cxx_std_23)

file(GLOB_RECURSE ZRENDERER_HEADERS CONFIGURE_DEPENDS
    "${ROOT_DIR}/include/ZokataRenderer/*.h"
    "${ROOT_DIR}/include/ZokataRenderer/*.hpp"
)
file(GLOB_RECURSE ZRENDERER_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/ZokataRenderer/*.cpp"
    "${ROOT_DIR}/src/ZokataRenderer/*.cc"
    "${ROOT_DIR}/src/ZokataRenderer/*.cxx"
)

add_library(Zokata-renderer STATIC
    ${ZRENDERER_SOURCES}
    ${ZRENDERER_HEADERS}
)

if(ZRENDERER_HEADERS)
    source_group(TREE "${ROOT_DIR}/include"
        PREFIX "include"
        FILES ${ZRENDERER_HEADERS})
endif()
if(ZRENDERER_SOURCES)
    source_group(TREE "${ROOT_DIR}/src"
        PREFIX "src"
        FILES ${ZRENDERER_SOURCES})
endif()

target_include_directories(Zokata-renderer
    PUBLIC
        ${ROOT_DIR}/include
    PRIVATE
        ${ROOT_DIR}/src  # private, non-exported headers
)

target_compile_definitions(Zokata-renderer
    PUBLIC
        GLFW_INCLUDE_VULKAN
        VK_USE_PLATFORM_WIN32_KHR
)

target_link_libraries(Zokata-renderer
    PUBLIC
        Vulkan::Vulkan
        ${GLFW_IMPORTED_TARGET}
        glm::glm
        imgui::imgui
        Zokata-math
)

target_compile_features(Zokata-renderer PUBLIC cxx_std_23)

file(GLOB_RECURSE ZENGINE_HEADERS CONFIGURE_DEPENDS
    "${ROOT_DIR}/include/ZokataEngine/*.h"
    "${ROOT_DIR}/include/ZokataEngine/*.hpp"
)
file(GLOB_RECURSE ZENGINE_SOURCES CONFIGURE_DEPENDS
    "${ROOT_DIR}/src/ZokataEngine/*.cpp"
    "${ROOT_DIR}/src/ZokataEngine/*.cc"
    "${ROOT_DIR}/src/ZokataEngine/*.cxx"
)

add_library(Zokata-engine STATIC
    ${ZENGINE_SOURCES}
    ${ZENGINE_HEADERS}
)

if(ZENGINE_HEADERS)
    source_group(TREE "${ROOT_DIR}/include"
        PREFIX "include"
        FILES ${ZENGINE_HEADERS})
endif()
if(ZENGINE_SOURCES)
    source_group(TREE "${ROOT_DIR}/src"
        PREFIX "src"
        FILES ${ZENGINE_SOURCES})
endif()

target_include_directories(Zokata-engine
    PUBLIC
        ${ROOT_DIR}/include
    PRIVATE
        ${ROOT_DIR}/src
)

target_link_libraries(Zokata-engine
    PUBLIC
        Zokata-renderer
        yaml-cpp
        Zokata-log
        Zokata-math
)

target_compile_features(Zokata-engine PUBLIC cxx_std_23)

set(ZOKATA_SOURCES
    ${ROOT_DIR}/src/ZOKATA/main.cpp
)

add_executable(ZOKATA
    ${ZOKATA_SOURCES}
)

source_group(TREE "${ROOT_DIR}/src"
    PREFIX "src"
    FILES ${ZOKATA_SOURCES})

target_link_libraries(ZOKATA
    PRIVATE
        Zokata-engine
)

target_compile_features(ZOKATA PRIVATE cxx_std_23)

message(STATUS "Using Vulkan SDK version: ${Vulkan_VERSION}")
