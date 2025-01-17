if(BUILDING_FOR_ELASTIC_NODE)
    add_library(Time Time.c)

    # IMPORTANT: This is an interface include because some host platforms are case insensitive
    # (MacOS). On these we would include Time.h and pico sdk, which is linked as interface libs
    # will include time.h. Due to case insensitivity pico would include our header instead of the
    # pico one.
    target_include_directories(Time INTERFACE include)
    target_link_libraries(Time PRIVATE Common Pico::pico_time Pico::hardware_timer Pico::pico_runtime)
else()
    add_library(Time INTERFACE)
    target_include_directories(Time INTERFACE include)
endif ()