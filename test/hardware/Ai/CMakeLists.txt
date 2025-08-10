if (BUILDING_FOR_ELASTIC_NODE)
    add_executable(TestMemoryUsageLinearForward TestMemoryUsageLinearForward.c)
    target_link_libraries(TestMemoryUsageLinearForward PRIVATE
            Common
            AiHelpers
            Linear

            Pico::pico_stdlib
            Pico::hardware_spi
            Pico::pico_bootrom
    )
    create_enV5_executable(TestMemoryUsageLinearForward)

    add_executable(TestMemoryUsageLinearBackward TestMemoryUsageLinearBackward.c)
    target_link_libraries(TestMemoryUsageLinearBackward PRIVATE
            Common
            AiHelpers
            Linear

            Pico::pico_stdlib
            Pico::hardware_spi
            Pico::pico_bootrom
    )
    create_enV5_executable(TestMemoryUsageLinearBackward)
endif ()