set(AVAILABLE_TARGETS
  HOST
  ENV5_1
  ENV5_2
  PICO
  PICOW
  PICO2
  PICO2W
)

if(NOT (ELASTIC_AI_TARGET IN_LIST AVAILABLE_TARGETS))
  message(FATAL_ERROR
    "You have to specify -DELASTIC_AI_TARGET=<target> with <target> being one of ${AVAILABLE_TARGETS}"
  )
elseif(${ELASTIC_AI_TARGET} STREQUAL HOST)
  include(${CMAKE_CURRENT_LIST_DIR}/targets/host.cmake)
elseif(${ELASTIC_AI_TARGET} STREQUAL ENV5_1)
  include(${CMAKE_CURRENT_LIST_DIR}/targets/env5.cmake)
elseif(${ELASTIC_AI_TARGET} STREQUAL ENV5_2)
  include(${CMAKE_CURRENT_LIST_DIR}/targets/env5.cmake)
elseif(${ELASTIC_AI_TARGET} STREQUAL PICO)
  include(${CMAKE_CURRENT_LIST_DIR}/targets/pico.cmake)
elseif(${ELASTIC_AI_TARGET} STREQUAL PICOW)
  include(${CMAKE_CURRENT_LIST_DIR}/targets/picow.cmake)
elseif(${ELASTIC_AI_TARGET} STREQUAL PICO2)
  include(${CMAKE_CURRENT_LIST_DIR}/targets/pico2.cmake)
elseif(${ELASTIC_AI_TARGET} STREQUAL PICO2W)
  include(${CMAKE_CURRENT_LIST_DIR}/targets/pico2w.cmake)
endif()

