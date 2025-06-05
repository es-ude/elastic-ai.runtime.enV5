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

  if(_BUILDING_FOR_HARDWARE)
    add_freertos()
  endif()

  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Werror=int-conversion>)
  add_compile_options(-ftrapv) # abort execution on integer overflow

  add_subdirectory(src)

  if (EAI_RUNTIME_TOP_LEVEL_PROJECT)
    add_subdirectory(test/hardware)
    add_subdirectory(test/unit)
  endif()

  if(_BUILDING_FOR_HARDWARE AND NOT TARGET tinyusb_device)
    message(WARNING "not building project, because tinyusb not initialized")
  endif()
endif()

