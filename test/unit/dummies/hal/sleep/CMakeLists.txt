add_library(SleepDummy Sleep.c)
target_link_libraries(SleepDummy PRIVATE Common)
target_link_libraries(SleepDummy PUBLIC Sleep)
