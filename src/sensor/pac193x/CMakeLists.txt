#add_elastic_ai_lib(
#        NAME Pac193x
#        SRCS Pac193x.c
#        DEPS
#            Common
#            Sleep
#            Gpio
#            I2c
#)

block(SCOPE_FOR VARIABLES)
    set(deps Common Sleep Gpio I2c)

    add_library(Pac193x__hdrs INTERFACE)
    add_library(Pac193x__impl Pac193x.c)
    elastic_ai_target_link_hdrs(Pac193x__impl ${deps})
    elastic_ai_target_link_impl(Pac193x__impl ${deps})
    add_library(Pac193x INTERFACE)

    elastic_ai_connect_libs(Pac193x)


endblock()