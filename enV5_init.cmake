include(${CMAKE_CURRENT_LIST_DIR}/helpers.cmake)

function(enV5_init_unit_tests)
    # include unit-test framework
    add_unity()

    # include required standard libs for testing
    add_cexception()
    add_runtime_c()
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/common)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/network/atCommands)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/network/config)

    # include dummies for local machine
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
