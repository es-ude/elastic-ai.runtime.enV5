function(fetch_git_submodules)
    find_package(Git QUIET)
    message("${PROJECT_SOURCE_DIR}/.git")
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update Submodules
        option(GIT_SUBMODULE "Check submodules build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}")
            endif()
        endif()
    endif()
endfunction()