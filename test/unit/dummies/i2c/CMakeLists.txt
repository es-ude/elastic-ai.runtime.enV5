add_library(I2cDummy I2c.c)
target_include_directories(I2cDummy PUBLIC include)
target_link_libraries(I2cDummy PUBLIC I2c)
