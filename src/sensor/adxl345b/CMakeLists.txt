add_library(Adxl345b Adxl345b.c)
target_include_directories(Adxl345b PUBLIC include)
target_link_libraries(Adxl345b PRIVATE
        Common
        Sleep
        Time
        I2c)
