add_library(HttpDummy)
target_sources(HttpDummy PRIVATE HTTP.c)
target_link_libraries(HttpDummy PRIVATE Common__hdrs CException__hdrs Http__hdrs)
target_include_directories(HttpDummy PUBLIC include)
