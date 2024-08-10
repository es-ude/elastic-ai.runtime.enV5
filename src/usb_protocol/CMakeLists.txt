add_library(usb-protocol STATIC
        ${CMAKE_CURRENT_LIST_DIR}/Protocol.c
)
target_include_directories(usb-protocol PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(usb-protocol PRIVATE
    checksum-xor
)