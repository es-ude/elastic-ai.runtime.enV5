cmake_minimum_required(VERSION 3.20...3.24)

if(NOT TARGET _eai_runtime_inclusion_marker)
  add_library(_eai_runtime_inclusion_marker INTERFACE)

  include(eai_runtime_init.cmake)

  project(enV5-runtime C CXX ASM)
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
  eai_runtime_is_top_level_project(EAI_RUNTIME_TOP_LEVEL_PROJECT)

  if(EAI_RUNTIME_TOP_LEVEL_PROJECT)
    init_pico_sdk()
    add_unity()
    add_ctest()
  endif()
  add_cexception()
  add_runtime_c()

  if(DEBUG_OUTPUT)
    add_definitions(-DDEBUG_MODE)
  endif()

  if(_BUILDING_FOR_HARDWARE)
    add_freertos()

    add_subdirectory(src)

    if(EAI_RUNTIME_TOP_LEVEL_PROJECT)
      add_subdirectory(test/hardware)
    endif()
  else()
    add_compile_options(-ftrapv) # abort execution on integer overflow

    add_subdirectory(src/common)
    add_subdirectory(src/filesystem)
    add_subdirectory(src/flash)
    add_subdirectory(src/fpga/fpgaConfigurationHandler)
    add_subdirectory(src/hal)
    add_subdirectory(src/network)
    add_subdirectory(src/rtos)
    add_subdirectory(src/sensor)
    add_subdirectory(src/usb_protocol)

    if(EAI_RUNTIME_TOP_LEVEL_PROJECT)
      add_subdirectory(test/unit)
    endif()
  endif()

  if(BUILDING_FOR_HARDWARE AND NOT TARGET tinyusb_device)
    message(WARNING "not building project, because tinyusb not initialized")
  endif()
endif()
