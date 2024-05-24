#
# LIBUDEV_FOUND
# LIBUDEV_INCLUDE_DIRS
# LIBUDEV_LIBRARIES

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(_LIBUDEV libudev QUIET)
endif()

find_path(LIBUDEV_INC
    NAMES libudev.h
    HINTS
        ${_LIBUDEV_INCLUDE_DIRS}
    HINTS
        ${CMAKE_SOURCE_DIR}/deps/
)

find_library(LIBUDEV_LIB
    NAMES ${_LIBUDEV_LIBRARIES} udev
    HINTS
        ${_LIBUDEV_LIBRARY_DIRS}
        ${_LIBUDEV_STATIC_LIBRARY_DIRS}
    HINTS
        ${CMAKE_SOURCE_DIR}/deps/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libudev DEFAULT_MSG LIBUDEV_LIB LIBUDEV_INC)
mark_as_advanced(LIBUDEV_INC LIBUDEV_LIB)

if(LIBUDEV_FOUND)
    set(LIBUDEV_INCLUDE_DIRS ${LIBUDEV_INC})
    set(LIBUDEV_LIBRARIES ${LIBUDEV_LIB})
endif()
