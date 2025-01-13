add_library(Pac193x Pac193x.c)
target_include_directories(Pac193x PUBLIC include)
target_link_libraries(Pac193x PRIVATE Common Sleep Gpio I2c)
