cmake_minimum_required(VERSION 3.28)
project(MacTegraRCM)

set(CMAKE_CXX_STANDARD 17)

file (GLOB MY_SRC
        "src/*"
        "src/fuseec/*"
        "src/backend/*")

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base)
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

add_compile_options(-stdlib=libc++ -framework Carbon)

set(CUR_TARGET MacTegraRCM)
set(MACOSX_BUNDLE_ICON_FILE icon.icns)

set(myApp_ICON resources/app/icon/icon.icns
        src/fusee.cpp
        src/fusee.h
)
set_source_files_properties(${myApp_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

add_executable(${CUR_TARGET} MACOSX_BUNDLE ${MY_SRC} ${myApp_ICON})
set_target_properties(${CUR_TARGET} PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_GUI_IDENTIFIER my.domain.style.identifier.${CUR_TARGET}
        MACOSX_BUNDLE_BUNDLE_NAME ${CUR_TARGET}
        MACOSX_BUNDLE_BUNDLE_VERSION ${ISOBERRY_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${ISOBERRY_VERSION}
)

# Link libraries
target_link_libraries(MacTegraRCM PRIVATE ${wxWidgets_LIBRARIES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0)

target_link_libraries(MacTegraRCM PRIVATE PkgConfig::libusb)

add_subdirectory("dep/fuseec")
target_link_libraries(MacTegraRCM PRIVATE fuseec)