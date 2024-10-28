add_elastic_ai_lib(
        NAME Flash
        SRCS Flash.c
        DEPS Common Sleep Spi
)

target_link_libraries(Flash__hdrs INTERFACE Spi__hdrs)