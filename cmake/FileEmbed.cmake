get_filename_component(bin_name ${bin_in} NAME)
string(REGEX REPLACE "\\.| |-" "_" bin_name ${bin_name})

set(c_out ${bin_name}.c)

file(READ ${bin_in} filedata HEX)

message(${c_out})

string(REGEX REPLACE  "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})

file(WRITE ${c_out} "#include <stddef.h>\n#include <stdint.h>\nconst uint8_t ${bin_name}[] = {${filedata}};\nconst size_t ${bin_name}_size = sizeof(${bin_name});\n")
