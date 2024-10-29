block(SCOPE_FOR VARIABLES)
    set(deps CException Common I2c)

    add_library(Sht3x__hdrs INTERFACE)
    add_library(Sht3x__impl Sht3x.c)
    add_library(Sht3x INTERFACE)

    elastic_ai_connect_libs(Sht3x)
    elastic_ai_target_link_impl(Sht3x__impl ${deps})
    elastic_ai_target_link_hdrs(Sht3x__impl ${deps})
endblock()