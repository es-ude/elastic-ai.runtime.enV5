if (NOT TARGET _eai_runtime_pre_init_marker)
    add_library(_eai_runtime_pre_init_marker INTERFACE)

    function(eai_runtime_is_top_level_project VAR)
        string(TOLOWER ${CMAKE_CURRENT_LIST_DIR} __list_dir)
        string(TOLOWER ${CMAKE_SOURCE_DIR} __source_dir)
        if (__source_dir STREQUAL __list_dir)
            set(${VAR} 1 PARENT_SCOPE)
        else ()
            set(${VAR} 0 PARENT_SCOPE)
        endif ()
    endfunction()

    if (NOT EAI_RUNTIME_PATH)
        set(EAI_RUNTIME_PATH ${CMAKE_CURRENT_LIST_DIR})
    endif ()

    get_filename_component(EAI_RUNTIME_PATH "${EAI_RUNTIME_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

    set(EAI_RUNTIME_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the EAI runtime" FORCE)

    list(APPEND CMAKE_MODULE_PATH ${EAI_RUNTIME_PATH}/cmake)

    message("EAI runtime path is ${EAI_RUNTIME_PATH}")

    include(third_party_deps)
    include(set_build_target)
    add_pico_sdk("2.1.1")

    macro(eai_runtime_init)
        if (NOT CMAKE_PROJECT_NAME)
            message(WARNING "eai_runtime_init() should be called after the project is created (and languages added)")
        endif ()
        # init_pico_sdk()
        add_subdirectory(${EAI_RUNTIME_PATH} eai-runtime)
    endmacro()
endif ()
