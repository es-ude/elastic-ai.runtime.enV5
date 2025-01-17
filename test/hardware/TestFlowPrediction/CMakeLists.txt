if(BUILDING_FOR_ELASTIC_NODE)
    add_executable(HardwareTestFlowPrediction HardwareTestFlowPrediction.c)
    target_link_libraries(HardwareTestFlowPrediction PRIVATE
            Common
            Flash
            FpgaConfigurationHandler
            EnV5HwController
            Middleware
            EnV5HwConfiguration

            Pico::hardware_spi
            Pico::pico_stdlib
    )
    create_enV5_executable(HardwareTestFlowPrediction)
endif()