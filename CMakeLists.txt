cmake_minimum_required(VERSION 3.20...3.24)

include(cmake/set_build_target.cmake)
include(cmake/third_party_deps.cmake)

add_pico_sdk("2.0.0")
add_unity()
add_cexception()
add_freertos()
add_runtime_c()



set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(MVC)
    add_compile_options(/W4)
else()
    add_compile_options(
            -Wall
            -Wextra
            -Wdouble-promotion # warn when float gets promoted to double as this can be quite expensive
            -Wnull-dereference
            -Wimplicit-fallthrough # warn about fall through in switch case
    )

endif ()

# enable debug prints
if (DEBUG_OUTPUT)
    add_definitions(-DDEBUG_MODE)
endif ()



if(NOT BUILDING_FOR_ELASTIC_NODE)
    add_compile_options(-ftrapv) # abort execution on integer overflow
endif ()

project(enV5 C CXX ASM)

# enable test execution via CMake
include(CTest)

# include unit-test framework
include(cmake/custom_targets.cmake)


pico_sdk_init()

include(cmake/pico_targets.cmake)
add_subdirectory(src)
add_subdirectory(test/unit)

#    if (TARGET tinyusb_device)
#        # include required libraries
#        add_basic_functionality()
#        add_freertos_kernel()
#
#        add_rp2040_hal()
#        add_sensor_libraries()
#
#        # include hardware tests
#        add_subdirectory(test/hardware)
#
#        add_dependency_graph()
#    elseif (PICO_ON_DEVICE)
#        message(WARNING "Not building Project, because TinyUSB submodule is not initialized in the SDK!")
#    endif ()
#endif ()
