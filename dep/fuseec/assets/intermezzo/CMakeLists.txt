set(IN "${CMAKE_CURRENT_SOURCE_DIR}/intermezzo.bin")
set(OUT "${CMAKE_CURRENT_BINARY_DIR}/intermezzo_bin.c")

add_custom_command(
    OUTPUT "${OUT}"
    COMMAND ${CMAKE_COMMAND}
    "-Dbin_in=${IN}"
    -P ${CMAKE_SOURCE_DIR}/cmake/FileEmbed.cmake
    DEPENDS "${IN}"
    VERBATIM
)

add_library(intermezzo OBJECT ${OUT})
