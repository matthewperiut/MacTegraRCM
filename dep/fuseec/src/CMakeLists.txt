find_package(libusb1 REQUIRED)
find_package(libudev)

set(CFLAGS
    -Wall -Wextra -Wfloat-equal -Wunreachable-code
    -Wcast-qual -Wswitch-default -Wconversion
    -Wshadow -Wstrict-aliasing -Winit-self -Wuninitialized
    -Wcast-align -Wpointer-arith -Wmissing-declarations
    -Wmissing-include-dirs -Wno-unused-parameter
)

add_subdirectory(backend)

list(APPEND
    FUSEEC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/payload.c
    ${CMAKE_CURRENT_SOURCE_DIR}/payload.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tegra.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tegra.h
    ${CMAKE_CURRENT_SOURCE_DIR}/usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/usb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util.h
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_library(fuseec STATIC ${FUSEEC_SOURCES})
target_compile_options(fuseec PRIVATE ${CFLAGS})

target_link_libraries(fuseec LINK_PUBLIC intermezzo)
target_link_libraries(fuseec LINK_PUBLIC backend)

target_include_directories(fuseec PUBLIC ${LIBUSB1_INCLUDE_DIRS})
target_link_libraries(fuseec LINK_PUBLIC ${LIBUSB1_LIBRARIES})

if (LIBUDEV_FOUND)
    target_compile_definitions(fuseec PRIVATE HAVE_UDEV)
    target_include_directories(fuseec PUBLIC ${LIBUDEV_INCLUDE_DIRS})
    target_link_libraries(fuseec LINK_PUBLIC ${LIBUDEV_LIBRARIES})
endif()

if (EMBED_USB_DATABASE)
    target_compile_definitions(fuseec PRIVATE HAVE_USB_DATABASE)
    target_link_libraries(fuseec LINK_PUBLIC usb_database)
endif()