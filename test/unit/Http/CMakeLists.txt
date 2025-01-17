add_library(test_lib_http__srcs INTERFACE)
target_include_directories(test_lib_http__srcs INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/network/esp/include
)
target_link_libraries(test_lib_http__srcs INTERFACE
        Http
        EspDummy
        CException
)

add_executable(UnitTestHttpNew UnitTestHttp.c)
target_link_libraries(UnitTestHttpNew PRIVATE test_lib_http__srcs unity)
add_test(UnitTestHttpNew UnitTestHttpNew)