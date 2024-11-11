################
# Functions in here are supposed to help keeping consistent naming scheme and package structure
# for different cmake targets.
# Use the functions add_elastic_ai_lib and add_elastic_ai_unit_test
################

function(elastic_ai_target_link_hdrs target)
    foreach (d ${ARGN})
        target_link_libraries(${target} PRIVATE ${d}__hdrs)
    endforeach ()
endfunction()


function(elastic_ai_target_link_impl target)
    foreach (d ${ARGN})
        target_link_libraries(${target} PRIVATE ${d}__impl)
    endforeach ()
endfunction()


function(elastic_ai_initialize_implementation target)
    target_link_libraries(${target}__impl PRIVATE ${target}__hdrs)
    target_include_directories(${target}__impl PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    foreach (dep ${ARGN})
        if( (NOT TARGET ${target}__impl) AND (NOT TARGET ${target}__hdrs) )
            message(WARNING "${target} features neither implementation nor headers and will not be used")
        else()
            target_link_libraries(${target}__impl PRIVATE ${dep}__hdrs)
        endif ()
    endforeach()
endfunction()

function(elastic_ai_connect_libs target)
    if(TARGET ${target}__impl)
        target_link_libraries(${target} INTERFACE ${target}__impl)
    endif ()
    target_link_libraries(${target} INTERFACE ${target}__hdrs)


endfunction()

function(add_elastic_ai_lib)
    # Takes the following parameters:
    #
    # NAME: the name of your library (withstand the urge to use 'lib' as a suffix!)
    # SRCS: all the *.c files that need to be compiled for this library
    # DEPS: all libaries, that the current library depends on. These need to be specified via
    #      the elastic_ai_lib function as well.
    #
    # We assume that public headers live inside an `include` directory in the package.
    #
    #
    # Example:
    #
    # elastic_ai_lib(NAME Flash
    #                SRCS Flash.c
    #                DEPS Common Spi Sleep)
    #
    # elastic_ai_lib(NAME NeedsFlash
    #                SRCS ImplementationThatNeedsFlash.c
    #                DEPS Flash Common Spi Sleep)
    #
    #
    #
    # Will generate several different library targets:
    #  1. <mylib>__hdrs : a pure INTERFACE library containing only the public headers of this
    #     module. These hdr only libs will be automatically linked when defining a new lib that depends
    #     on the current one.
    #  2. <mylib>__impl : a library that contains the implementation using the package root dir as private include directory.
    #  3. <mylib> : an INTERFACE library bundling the *__hdrs and *__impl.
    #
    # If you need to link a lib A against a library B that was not defined as an elastic_ai_lib
    # you will want to do it like this in most cases:
    #
    # elastic_ai(NAME A SRCS A.c)
    # target_include_directories(A__impl PRIVATE $<TARGET_PROPERTY:B,INTERFACE_INCLUDE_DIRECTORIES>))
    #
    # IMPORTANT: Note that this approach will not work to include directories of libraries that
    # are linked against B.
    set(options HW_ONLY HOST_ONLY)
    set(oneValueArgs NAME)
    set(multiValueArgs SRCS DEPS)
    cmake_parse_arguments(PARSE_ARGV 0 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )
    set(BUILD_IMPLEMENTATION ((NOT ${arg_HW_ONLY}) OR BUILDING_FOR_ELASTIC_NODE))

    add_library(${arg_NAME} INTERFACE)
    add_library(${arg_NAME}__hdrs INTERFACE)
    target_include_directories(${arg_NAME}__hdrs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
    if(${BUILD_IMPLEMENTATION})
        add_library(${arg_NAME}__impl ${arg_SRCS})
        elastic_ai_initialize_implementation(${arg_NAME} ${arg_DEPS})
    endif ()

    elastic_ai_connect_libs(${arg_NAME})

endfunction()

function(initialize_unit_test lib_under_test)

endfunction()


function(add_elastic_ai_unit_test)
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


    add_executable(${NAME} EXCLUDE_FROM_ALL UnitTest${arg_LIB_UNDER_TEST}.c)
    target_sources(${NAME} PRIVATE ${arg_MORE_SOURCES})
    target_link_libraries(${NAME} PRIVATE ${arg_LIB_UNDER_TEST} unity::framework ${arg_MORE_LIBS})
    add_test(${NAME} ${NAME})
    set_property(TEST ${NAME} PROPERTY LABELS unit)
endfunction()



function(create_enV5_executable target)
    # enable usb output
    pico_enable_stdio_usb(${target} 1)
    # disable uart output
    pico_enable_stdio_uart(${target} 0)
    # create map/bin/hex/uf2 file etc.
    pico_add_uf2_output(${target})
    #    # copy u2f files after build to out directory
    #    file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    #    add_custom_command(TARGET ${target} POST_BUILD
    #            COMMAND ${CMAKE_COMMAND} -E copy
    #            ${CMAKE_BINARY_DIR}/${relative_path}/${target}.uf2
    #            ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}-Rev${REVISION}/${relative_path}/${target}.uf2)
endfunction()


function(add_elastic_ai_hardware_test)

    set(options HW_ONLY HOST_ONLY)
    set(oneValueArgs NAME)
    set(multiValueArgs SRCS DEPS)
    cmake_parse_arguments(PARSE_ARGV 0 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )
    add_executable(${arg_NAME} ${CMAKE_CURRENT_LIST_DIR}/${arg_NAME}.c)
    create_enV5_executable(${arg_NAME} ${CMAKE_CURRENT_LIST_DIR}/${arg_NAME}.c)

    foreach (dep ${arg_DEPS})
        target_link_libraries(${arg_NAME} PRIVATE ${dep})
    endforeach()
endfunction()