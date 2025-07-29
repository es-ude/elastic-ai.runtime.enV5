add_library(TimeDummy Time.c)
target_link_libraries(TimeDummy PRIVATE Common)
target_link_libraries(TimeDummy PUBLIC Time)
