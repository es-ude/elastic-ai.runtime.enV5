add_library(flash_lib INTERFACE)
target_include_directories(flash_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(flash_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Flash.c)
target_link_libraries(flash_lib INTERFACE
        common_lib
        spi_interface)