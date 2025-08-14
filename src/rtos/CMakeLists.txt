if(_BUILDING_FOR_HARDWARE)
  add_library(freeRtosUtils
    FreeRtosTaskWrapper.c
    FreeRtosSMP.c
    FreeRtosQueueWrapper.c
    FreeRtosMutexWrapper.c
  )
  target_include_directories(freeRtosUtils PUBLIC include)
  target_link_libraries(freeRtosUtils PRIVATE
    Common
    Sleep
    Pico::pico_bootrom
    Pico::pico_stdlib
    Pico::hardware_base
    freeRtos::FreeRTOS-Kernel
    freeRtos::FreeRTOS-Kernel-Heap3
  )
else()
  add_library(freeRtosUtils INTERFACE)
  target_include_directories(freeRtosUtils INTERFACE include)
endif()
