function(__register_target_as_unit_test target)
    if(NOT TARGET all_unit_tests)
        add_custom_target(all_unit_tests)
    endif ()
    add_dependencies(all_unit_tests ${target})

    add_test(${target} ${target})
    set_property(TEST ${target} PROPERTY LABELS unit)
endfunction()

function(add_elastic_ai_unit_test)
    # You only need to specify the LIB_UNDER_TEST.
    # The function will automatically try to compile a file with the same name, but
    # the prefix `Unittest` and link against LIB_UNDER_TEST.
    # For flexibility you can specify additional libraries (MORE_LIBS) and
    # sources (MORE_SOURCES).
    set(oneValueArgs LIB_UNDER_TEST)
    set(multiValueArgs MORE_SOURCES MORE_LIBS)
    cmake_parse_arguments(PARSE_ARGV 0 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )
    string(CONCAT NAME "UnitTest" ${arg_LIB_UNDER_TEST})
    if(NOT arg_MORE_SOURCES)
        set(arg__MORE_SOURCES "")
    endif ()

    if(NOT arg_MORE_LIBS)
        set(arg_MORE_LIBS "")
    endif ()

    if (NOT arg_DEPS)
        set(arg_DEPS "")
    endif ()


    add_executable(${NAME} ${NAME}.c)
    target_link_libraries(${NAME} PRIVATE
            ${arg_LIB_UNDER_TEST}
            unity::framework
            ${arg_MORE_LIBS})
    __register_target_as_unit_test(${NAME})

endfunction()

