################
# Functions in here are supposed to help keeping consistent naming scheme and package structure
# for different cmake targets.
################

function(elastic_ai_lib)
    # Takes the following parameters:
    #
    # NAME: the name of your library (withstand the urge to use 'lib' as a suffix!)
    # SOURCES: all the *.c files that need to be compiled for this library
    # DEPS: all libaries, that the current library depends on. These need to be specified via
    #      the elastic_ai_lib function as well.
    #
    #
    # Example:
    #
    # elastic_ai_lib(NAME Flash
    #                SOURCES Flash.c
    #                DEPS Common Spi Sleep)
    #
    # elastic_ai_lib(NAME NeedsFlash
    #                SOURCES ImplementationThatNeedsFlash.c
    #                DEPS Flash Common Spi Sleep)
    #
    #
    # We assume that public headers live inside an `include` directory in the package.
    #
    #
    # Will generate several different library targets:
    #  1. <mylib>__hdrs : a pure INTERFACE library containing only the public headers of this
    #     module. These hdr only libs will be automatically linked when defining a new lib that depends
    #     on the current one.
    #  2. <mylib> : a library that contains the implementation in addition to 1, linking against all
    #     all hdr libs of the specified dependencies.
    #
    # If you need to link a lib A against a library B that was not defined as an elastic_ai_lib
    # you will want to do it like this in most cases:
    #
    # elastic_ai_lib(NAME A SOURCES A.c)
    # target_link_libraries(A PRIVATE B)
    # target_include_directories(A PRIVATE get_property(TARGET B INTERFACE_INCLUDE_DIRECTORY))
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES DEPS)
    cmake_parse_arguments(PARSE_ARGV 0 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )
    if(NOT arg_DEPS)
        set(arg_DEPS "")
    endif ()
    add_library(${arg_NAME}__hdrs INTERFACE)
    target_include_directories(${arg_NAME}__hdrs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

    add_library(${arg_NAME} ${arg_SOURCES})
    target_include_directories(${arg_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    set(hdrLibs "")
    foreach (lib ${DEPS})
        list(APPEND hdrLibs ${lib}__hdrs)
    endforeach ()
    target_link_libraries(${arg_NAME} PUBLIC ${arg_NAME}__hdrs)
    target_link_libraries(${arg_NAME} PRIVATE ${hdrLibs})

endfunction()


function(elastic_ai_unit_test)
    # Similar to elastic_ai_lib but defines a new unit test.
    # You only need to specify the LIB_UNDER_TEST.
    # The function will automatically try to compile a file with the same name, but
    # the prefix `Unittest` and link against the nodeps version of LIB_UNDER_TEST.
    # For flexibility you can specify additional libraries (MORE_LIBS) and
    # sources (MORE_SOURCES).
    set(oneValueArgs LIB_UNDER_TEST)
    set(multiValueArgs MORE_SOURCES MORE_LIBS)
    cmake_parse_arguments(PARSE_ARGV 0 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )
    if(NOT arg_MORE_SOURCES)
        set(arg__MORE_SOURCES "")
    endif ()

    if(NOT arg_MORE_LIBS)
        set(arg_MORE_LIBS "")
    endif ()

    if (NOT arg_DEPS)
        set(arg_DEPS "")
    endif ()
    add_executable(unit-test_${arg_LIB_UNDER_TEST} Unittest${arg_LIB_UNDER_TEST}.c)
    target_sources(unit-test_${arg_LIB_UNDER_TEST} PRIVATE ${arg_MORE_SOURCES})
    target_link_libraries(unit-test_${arg_LIB_UNDER_TEST} ${arg_LIB_UNDER_TEST} unity)
    add_test(unit-test_${arg_LIB_UNDER_TEST} unit-test_${arg_LIB_UNDER_TEST})
    target_link_libraries(unit-test_${arg_LIB_UNDER_TEST} ${arg_MORE_LIBS})
endfunction()
