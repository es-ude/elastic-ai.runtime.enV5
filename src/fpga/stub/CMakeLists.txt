## GENERIC STUB
add_library(stub_lib INTERFACE)
target_include_directories(stub_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(stub_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/stub.c)
target_link_libraries(stub_lib INTERFACE
        common_lib
        sleep_interface
        middleware_lib)

## STUB FOR SKELETON V1
add_library(stub_v1_lib INTERFACE)
target_include_directories(stub_v1_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include_v1)
target_sources(stub_v1_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/stub.c)
target_link_libraries(stub_v1_lib INTERFACE
        common_lib
        sleep_interface
        middleware_lib)

## STUB FOR SKELETON V2
add_library(stub_v2_lib INTERFACE)
target_include_directories(stub_v2_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include_v2)
target_sources(stub_v2_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/stub.c)
target_link_libraries(stub_v2_lib INTERFACE
        common_lib
        sleep_interface
        middleware_lib)
