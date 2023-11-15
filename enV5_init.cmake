include(${CMAKE_CURRENT_LIST_DIR}/helpers.cmake)

function(enV5_init_unit_tests)
    # enable test execution via CMake
    include(CTest)

    # include unity as unit-test framework
    add_unity()

    # add CException for libraries that use it
    add_cexception()

    # add dummies for unit tests
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/test/unit/dummies)
endfunction()

function(enV5_init)
    # include basic functionality for enV5
    add_basic_functionality()

    # add HAL for enV5
    add_rp2040_hal()

    # provide access to additional sensor libraries
    add_sensor_libraries()
endfunction()
