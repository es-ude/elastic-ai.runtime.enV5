add_library(filesystem_lib INTERFACE
        FileSystem.c)
target_sources(filesystem_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FileSystem.c)
target_include_directories(filesystem_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(filesystem_lib INTERFACE
        common_lib
        spi_interface)

