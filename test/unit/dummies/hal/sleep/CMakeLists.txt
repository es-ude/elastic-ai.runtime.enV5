add_library(SleepDummy Sleep.c)
target_include_directories(SleepDummy PUBLIC include)
target_link_libraries(SleepDummy PRIVATE Common)
target_link_libraries(SleepDummy PUBLIC Sleep)
