add_library(Sht3x Sht3x.c)
target_include_directories(Sht3x PUBLIC include)
target_link_libraries(Sht3x PRIVATE Common CException I2c)
