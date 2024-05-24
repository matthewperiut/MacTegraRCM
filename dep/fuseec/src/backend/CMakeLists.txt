find_package(libusb1)

if(LINUX)
    set(BACKEND "linux")
elseif(APPLE OR FREEBSD)
    set(BACKEND "libusb")
else()
    message(FATAL_ERROR "Could not find a suitable backend")
endif()

message(STATUS "Backend: ${BACKEND}")

list(APPEND
    BACKEND_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${BACKEND}.c
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.h
)

add_library(backend OBJECT ${BACKEND_SOURCES})
target_compile_options(backend PRIVATE ${CFLAGS})

target_include_directories(backend PUBLIC ${LIBUSB1_INCLUDE_DIRS})
target_link_libraries(backend LINK_PUBLIC ${LIBUSB1_LIBRARIES})

target_include_directories(backend PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_definitions(backend PUBLIC BACKEND=${BACKEND}_)
