add_library(SleepDummy)
target_sources(SleepDummy PRIVATE Sleep.c)
target_link_libraries(SleepDummy Sleep__hdrs Common__hdrs)