add_library(espBroker_lib INTERFACE)
target_sources(espBroker_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/espBroker.c)
target_include_directories(espBroker_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(espBroker_lib INTERFACE
        elastic-ai.runtime.c
        )
