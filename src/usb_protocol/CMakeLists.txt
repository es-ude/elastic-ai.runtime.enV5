add_library(usb-protocol STATIC
        ${CMAKE_CURRENT_LIST_DIR}/Protocol.c
        ${CMAKE_CURRENT_LIST_DIR}/DefaultCommands.c
)
target_include_directories(usb-protocol PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(usb-protocol PUBLIC
        CException
)