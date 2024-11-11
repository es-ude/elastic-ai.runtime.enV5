
add_library(Network Network.c)
target_include_directories(Network PUBLIC include)
target_link_libraries(Network PRIVATE freeRtosUtils Common Uart Esp Http NetworkConfig AtCommands)
