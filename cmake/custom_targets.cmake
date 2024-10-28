################
# Functions in here are supposed to help keeping consistent naming scheme and package structure
# for different cmake targets.
# Use the functions add_elastic_ai_lib and add_elastic_ai_unit_test
################


function(__target_use_interfaces target)
    foreach (arg ${ARGN})
        target_link_libraries(${target} PRIVATE ${arg}__hdrs)
    endforeach ()
endfunction()

function(__target_add_default_includes target)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
endfunction()

function(__add_elastic_ai_implementation)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES DEPS)
    cmake_parse_arguments(PARSE_ARGV 0 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )
    add_library(${arg_NAME} ${arg_SOURCES})
    target_include_directories(${arg_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
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
    set(BUILD_IMPLEMENTATION (arg_SRCS AND ((NOT ${arg_HW_ONLY}) OR BUILDING_FOR_ELASTIC_NODE)))
    if(${BUILD_IMPLEMENTATION})
        __add_elastic_ai_implementation(NAME ${arg_NAME}__impl SOURCES ${arg_SRCS})
        __target_use_interfaces(${arg_NAME}__impl ${arg_DEPS})
    endif ()

    add_library(${arg_NAME}__hdrs INTERFACE)
    target_include_directories(${arg_NAME}__hdrs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

    add_library(${arg_NAME} INTERFACE)

    if(${BUILD_IMPLEMENTATION})
        target_link_libraries(${arg_NAME} INTERFACE ${arg_NAME}__impl)
    endif ()
    target_link_libraries(${arg_NAME} INTERFACE ${arg_NAME}__hdrs)
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

    add_executable(${NAME} UnitTest${arg_LIB_UNDER_TEST}.c)
    target_sources(${NAME} PRIVATE ${arg_MORE_SOURCES})
    target_link_libraries(${NAME} ${arg_LIB_UNDER_TEST} unity::framework)
    add_test(${NAME} ${NAME})
    set_property(TEST ${NAME} PROPERTY LABELS unit)
    target_link_libraries(${NAME} ${arg_MORE_LIBS})
endfunction()