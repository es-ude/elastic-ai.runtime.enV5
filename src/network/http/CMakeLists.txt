add_library(Http HTTP.c)
target_include_directories(Http PUBLIC include)
target_link_libraries(Http PRIVATE Common CException AtCommands Esp)
