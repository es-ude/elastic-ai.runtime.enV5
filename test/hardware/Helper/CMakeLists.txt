################## hardwareTestHelper ##################
add_library(hardware-testHelper INTERFACE)
target_sources(hardware-testHelper INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/HardwaretestHelper.c)
target_include_directories(hardware-testHelper INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(hardware-testHelper INTERFACE
        common_lib
        pico_stdlib
        espBroker_lib
        network_lib
        freeRtosUtils)

