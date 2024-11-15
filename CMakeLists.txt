cmake_minimum_required(VERSION 3.20...3.24)

include(cmake/set_build_target.cmake)


include(cmake/third_party_deps.cmake)

add_pico_sdk("2.0.0")
add_unity()
add_cexception()
add_runtime_c()



set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if (DEBUG_OUTPUT)
    add_definitions(-DDEBUG_MODE)
endif ()



if(NOT BUILDING_FOR_ELASTIC_NODE)
    add_compile_options(-ftrapv) # abort execution on integer overflow
endif ()

project(enV5 C CXX ASM)
pico_sdk_init()

include(cmake/custom_targets.cmake)
include(cmake/pico_targets.cmake)

include(CTest)

add_subdirectory(src)
add_subdirectory(test/unit)
add_subdirectory(test/hardware/Helper)
add_subdirectory(test/hardware/Sensors)
add_subdirectory(test/hardware/TestConfiguration)
add_subdirectory(test/hardware/TestFlash)
add_subdirectory(test/hardware/TestFlashFileSystem)
add_subdirectory(test/hardware/TestFlowPrediction)
add_subdirectory(test/hardware/TestFreeRTOS)
add_subdirectory(test/hardware/TestMiddleware)
add_subdirectory(test/hardware/TestStub)
add_subdirectory(test/hardware/TestUsbProtocol)
add_freertos()

if(BUILDING_FOR_ELASTIC_NODE AND NOT TARGET tinyusb_device)
    message(WARNING "not building project, because tinyusb not initialized")
else()
endif ()

