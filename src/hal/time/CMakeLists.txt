if(BUILDING_FOR_ELASTIC_NODE)
    add_library(Time Time.c)

    target_include_directories(Time PUBLIC include)
    target_link_libraries(Time PRIVATE
        Common
        Pico::pico_time
        Pico::hardware_timer
        Pico::pico_runtime
    )
else()
    add_library(Time INTERFACE)
    target_include_directories(Time INTERFACE include)
endif ()
