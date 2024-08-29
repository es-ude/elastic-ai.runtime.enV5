add_library(usb-protocol STATIC
        ${CMAKE_CURRENT_LIST_DIR}/Protocol.c
        ${CMAKE_CURRENT_LIST_DIR}/DefaultCommands.c
        ${CMAKE_CURRENT_LIST_DIR}/Tools.c
)
target_include_directories(usb-protocol PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_directories(usb-protocol PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/internal
)
target_link_libraries(usb-protocol PUBLIC
        CException
)
target_link_libraries(usb-protocol PRIVATE
        enV5_hw_controller
        gpio_interface
        flash_lib
        middleware_lib
        stub_v2_lib
)