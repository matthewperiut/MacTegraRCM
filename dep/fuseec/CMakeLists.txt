cmake_minimum_required(VERSION 3.18)

project(fuseec VERSION 0.0.0 LANGUAGES C)

if(CMAKE_POLICY_DEFAULT_CMP0017 OR CMAKE_POLICY_DEFAULT_CMP0020)
    # touch these to remove warnings
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if("${CMAKE_SYSTEM_NAME}" EQUAL "Linux")
    set(LINUX 1 CACHE INTERNAL)
elseif("${CMAKE_SYSTEM_NAME}" EQUAL "FreeBSD")
    set(FREEBSD 1 CACHE INTERNAL)
endif()

if (WIN32)
    set(EMBED_USB_DATABASE_DEFAULT ON)
else()
    set(EMBED_USB_DATABASE_DEFAULT OFF)
endif()

option(BUILD_CLI "Build the CLI client" ON)
option(BUILD_QT "Build the Qt GUI client" OFF)
option(EMBED_USB_DATABASE "" ${EMBED_USB_DATABASE_DEFAULT})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_compile_definitions(VERSION=${CMAKE_PROJECT_VERSION})

add_subdirectory(assets)
add_subdirectory(src)
