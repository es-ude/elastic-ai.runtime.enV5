add_library(TimeDummy Time.c)
target_link_libraries(TimeDummy PUBLIC Time)
target_link_libraries(TimeDummy PRIVATE Common)