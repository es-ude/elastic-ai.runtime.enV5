add_library(test_lib_fpga_config_handler__srcs INTERFACE)
target_include_directories(test_lib_fpga_config_handler__srcs INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/hal/common/include
        ${CMAKE_CURRENT_LIST_DIR}/src/flash/include
        ${CMAKE_CURRENT_LIST_DIR}/src/network/http/include
)
target_link_libraries(test_lib_fpga_config_handler__srcs INTERFACE
        Common
        FpgaConfigurationHandler
        HttpDummy
        FlashDummy
)

add_executable(UnitTestFpgaConfigurationHandlerNew UnitTestFpgaConfigurationHandler.c)
target_link_libraries(UnitTestFpgaConfigurationHandlerNew PRIVATE
        test_lib_fpga_config_handler__srcs
        unity
)
add_test(UnitTestFpgaConfigurationHandlerNew UnitTestFpgaConfigurationHandlerNew)