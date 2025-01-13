add_library(Flash Flash.c)
target_include_directories(Flash PUBLIC include)
target_link_libraries(Flash PRIVATE Common Sleep)
target_link_libraries(Flash PUBLIC Spi)
