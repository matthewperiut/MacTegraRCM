set(IN "${CMAKE_CURRENT_BINARY_DIR}/usb.ids")
set(OUT "${CMAKE_CURRENT_BINARY_DIR}/usb_ids.c")

file(DOWNLOAD "http://www.linux-usb.org/usb.ids" "${IN}")


add_custom_command(
    OUTPUT "${OUT}"
    COMMAND ${CMAKE_COMMAND}
    "-Dbin_in=${IN}"
    -P ${CMAKE_SOURCE_DIR}/cmake/FileEmbed.cmake
    DEPENDS "${IN}"
    VERBATIM
)

add_library(usb_database OBJECT ${OUT})
