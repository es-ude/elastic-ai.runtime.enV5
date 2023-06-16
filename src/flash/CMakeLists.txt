add_library(flash_lib INTERFACE)
target_sources(flash_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Flash.c
        ${CMAKE_CURRENT_LIST_DIR}/FlashWrite.c
        ${CMAKE_CURRENT_LIST_DIR}/FileIterator.c)
target_include_directories(flash_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(flash_lib INTERFACE spi_lib
        pico_stdlib)