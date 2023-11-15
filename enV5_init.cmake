include(helpers.cmake)

function(enV5_init)
    # include unity as unit-test framework
    add_unity()

    # include libraries
    add_basic_functionality()
    add_rp2040_hal()
    add_sensor_libraries()
endfunction()
