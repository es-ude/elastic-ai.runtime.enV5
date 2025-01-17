add_library(Middleware middleware.c)
target_include_directories(Middleware PUBLIC include)

target_link_libraries(Middleware PRIVATE
        Common
        Qxi
        Gpio
        EnV5HwController
)