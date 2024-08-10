add_library(checksum-xor STATIC
        ${CMAKE_CURRENT_LIST_DIR}/Xor.c
)
target_include_directories(checksum-xor PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
