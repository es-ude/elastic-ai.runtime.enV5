add_library(GpioDummy Gpio.c)
target_include_directories(GpioDummy PUBLIC include)
target_link_libraries(GpioDummy PRIVATE Gpio)

