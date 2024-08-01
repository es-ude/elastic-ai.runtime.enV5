add_library(file_system INTERFACE
        FileSystem.c)
target_sources(file_system INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FileSystem.c)
target_include_directories(file_system INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(file_system INTERFACE
        common_lib
        spi_interface)

