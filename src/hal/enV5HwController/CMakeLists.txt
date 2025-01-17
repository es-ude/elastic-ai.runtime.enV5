if(BUILDING_FOR_ELASTIC_NODE)
    add_library(EnV5HwController EnV5HwController.c)
    target_include_directories(EnV5HwController PUBLIC include)
    target_link_libraries(EnV5HwController PRIVATE
            Common
            EnV5HwConfiguration
            Gpio
            Sleep
    )
else()
    add_library(EnV5HwController INTERFACE)
    target_include_directories(EnV5HwController INTERFACE include)
endif ()

