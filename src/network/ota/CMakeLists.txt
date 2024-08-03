add_library(ota_lib INTERFACE)
target_include_directories(ota_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(ota_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Ota.c)
target_link_libraries(ota_lib INTERFACE
        freeRtosUtils
        common_lib
        espBroker_lib
        http_lib
        pico_flash
        flashLoader_lib)
