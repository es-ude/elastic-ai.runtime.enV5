add_library(HttpDummy HTTP.c)
target_include_directories(HttpDummy PUBLIC include)
target_link_libraries(HttpDummy PUBLIC Http)
target_link_libraries(HttpDummy PRIVATE CException)
