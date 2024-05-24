#
# LIBUSB1_FOUND
# LIBUSB1_INCLUDE_DIRS
# LIBUSB1_LIBRARIES

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(_LIBUSB1 libusb-1.0 QUIET)
endif()

find_path(LIBUSB1_INC
    NAMES libusb.h
    HINTS
        ${_LIBUSB1_INCLUDE_DIRS}
    HINTS
        ${CMAKE_SOURCE_DIR}/deps/
)

find_library(LIBUSB1_LIB
    NAMES ${_LIBUSB1_LIBRARIES} usb-1.0 libusb-1.0
    HINTS
        ${_LIBUSB1_LIBRARY_DIRS}
        ${_LIBUSB1_STATIC_LIBRARY_DIRS}
    HINTS
        ${CMAKE_SOURCE_DIR}/deps/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libusb1 DEFAULT_MSG LIBUSB1_LIB LIBUSB1_INC)
mark_as_advanced(LIBUSB1_INC LIBUSB1_LIB)

if(LIBUSB1_FOUND)
    set(LIBUSB1_INCLUDE_DIRS ${LIBUSB1_INC})
    set(LIBUSB1_LIBRARIES ${LIBUSB1_LIB})
endif()
