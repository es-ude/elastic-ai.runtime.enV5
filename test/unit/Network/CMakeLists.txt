add_library(test_lib_network__srcs INTERFACE)
target_include_directories(test_lib_network__srcs INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/network/esp/include
        ${CMAKE_CURRENT_LIST_DIR}/src/rtos/include
)
target_link_libraries(test_lib_network__srcs INTERFACE
        Network
        RtosDummy
        EspDummy

)

add_executable(UnitTestNetworkNew UnitTestNetwork.c)
target_link_libraries(UnitTestNetworkNew PRIVATE test_lib_network__srcs unity)
add_test(UnitTestNetworkNew UnitTestNetworkNew)