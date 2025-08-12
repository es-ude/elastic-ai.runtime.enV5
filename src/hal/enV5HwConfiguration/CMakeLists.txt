add_library(EnV5HwConfiguration INTERFACE)

if(NOT DEFINED _EAI_REVISION)
  if(NOT DEFINED HW_CONFIG_PATH)
    message(FATAL_ERROR "Missing hardware configuration")
  elseif(IS_ABSOLUTE ${HW_CONFIG_PATH})
    if(${REVISION_FILEPATH} MATCHES "\\/EnV5HwConfiguration\\.h$")
      target_include_directories(EnV5HwConfiguration INTERFACE
        HW_CONFIG_PATH
      )
    else()
      message(FATAL_ERROR
        "Provided file needs to be named EnV5HwConfiguration.h: ${HW_CONFIG_PATH}"
      )
    endif()
  endif()
elseif(${_EAI_REVISION} STREQUAL "1")
  target_include_directories(EnV5HwConfiguration INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/revision01
  )
elseif(${_EAI_REVISION} STREQUAL "2")
  target_include_directories(EnV5HwConfiguration INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/revision02
  )
endif()

