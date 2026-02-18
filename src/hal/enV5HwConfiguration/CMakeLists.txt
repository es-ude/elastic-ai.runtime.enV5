add_library(EnV5HwConfiguration INTERFACE)

if(NOT DEFINED _EAI_REVISION)
  message(STATUS "Using custom Hardware Configuration at ${HW_CONFIG_PATH}")
  if(NOT DEFINED HW_CONFIG_PATH)
    message(FATAL_ERROR "Missing hardware configuration")
  elseif(
    (IS_ABSOLUTE ${HW_CONFIG_PATH}) AND
    (EXISTS ${HW_CONFIG_PATH}) AND
    (IS_DIRECTORY ${HW_CONFIG_PATH})
  )
    if(NOT EXISTS "${HW_CONFIG_PATH}/eai/hal/EnV5HwConfiguration.h")
      message(FATAL_ERROR
        "Provided directory needs to contain '/eai/hal/EnV5HwConfiguration.h'"
      )
    else()
      target_include_directories(EnV5HwConfiguration INTERFACE
        ${HW_CONFIG_PATH}
      )
    endif()
  else()
    message(FATAL_ERROR
      "Please provide an absolute path to the hardware configuration directory"
    )
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
