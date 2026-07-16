if (_BUILDING_FOR_HARDWARE)

    add_library(config env5.c)
    target_link_libraries(config 
        hal2_I2c
    )
endif()