add_library(modelStore_lib INTERFACE)
target_sources(modelStore_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/ModelStore.c)
target_include_directories(network_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(modelStore_lib INTERFACE
        esp_lib
        freeRtosUtils
        network_lib)
