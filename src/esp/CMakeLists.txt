#
# Created by Lukas Post
#

add_library(esp_lib INTERFACE)
target_sources(esp_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/espBase.c
        ${CMAKE_CURRENT_LIST_DIR}/espTCP.c
        ${CMAKE_CURRENT_LIST_DIR}/espMQTT.c)
target_include_directories(esp_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(esp_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(esp_lib INTERFACE
        FreeRTOSUtils
        pico_stdlib
        uart_lib)
