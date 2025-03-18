set(AVAILABLE_TARGETS HOST ENV5_REV1 ENV5_REV2)

if(NOT (ELASTIC_AI_TARGET IN_LIST AVAILABLE_TARGETS))
    message(FATAL_ERROR "You have to specify -DELASTIC_AI_TARGET=<target> with <target> being one of ${AVAILABLE_TARGETS}")
elseif (${ELASTIC_AI_TARGET} STREQUAL HOST)
    include(${CMAKE_CURRENT_LIST_DIR}/host.cmake)
else ()
    include(${CMAKE_CURRENT_LIST_DIR}/env5.cmake)
endif ()

