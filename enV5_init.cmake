include(${CMAKE_CURRENT_LIST_DIR}/helpers.cmake)

function(enV5_init_unit_tests)
    # enable test execution via CMake
    include(CTest)

    # include unit-test framework
    add_unity()

    # include required standard libs for testing
    add_cexception()
    add_runtime_c()
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/common)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/network/atCommands)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/network/config)

    # include dummies for local machine
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/test/unit/dummies)

    # include our own libraries to test
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/sensor/sht3x)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/sensor/adxl345b)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/sensor/pac193x)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/network/wifi)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/network/http)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/network/broker)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/fpga/fpgaConfigurationHandler)
    # include our own unit-tests
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/test/unit)
endfunction()

function(enV5_init)
    # include basic functionality for enV5
    add_basic_functionality()

    # add HAL for enV5
    add_rp2040_hal()

    # provide access to additional sensor libraries
    add_sensor_libraries()

    # include our own hardware-tests
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/test/hardware)
endfunction()
