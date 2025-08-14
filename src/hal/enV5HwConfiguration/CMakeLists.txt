add_library(EnV5HwConfiguration INTERFACE)

if(NOT DEFINED _EAI_REVISION)
  if(NOT DEFINED HW_CONFIG_PATH)
    message(FATAL_ERROR "Missing hardware configuration")
  endif()
  message("Loading EnV5HwConfiguration.h from: ${HW_CONFIG_PATH}")
  target_include_directories(EnV5HwConfiguration INTERFACE
    HW_CONFIG_PATH
  )
elseif(${_EAI_REVISION} STREQUAL "1")
  target_include_directories(EnV5HwConfiguration INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/revision01
  )
elseif(${_EAI_REVISION} STREQUAL "2")
  target_include_directories(EnV5HwConfiguration INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/revision02
  )
endif()
