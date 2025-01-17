add_library(EspDummy Esp.c)
target_include_directories(EspDummy PUBLIC include)
target_link_libraries(EspDummy PRIVATE Common)
target_link_libraries(EspDummy PUBLIC Esp)

