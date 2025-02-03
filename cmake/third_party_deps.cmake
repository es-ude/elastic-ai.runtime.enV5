include(FetchContent)

# include pico-sdk
macro(add_pico_sdk tag)
  set(PICO_SDK_FETCH_FROM_GIT on)
  set(PICO_SDK_FETCH_FROM_GIT_TAG ${tag})
  set(PICOTOOL_FETCH_FROM_GIT_PATH ${CMAKE_SOURCE_DIR}/picotool)
  include(${CMAKE_SOURCE_DIR}/cmake/pico_sdk_import.cmake)
endmacro()

function(add_cexception)
  FetchContent_Declare(
    cexception
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/CException.git
    GIT_TAG v1.3.3)
  FetchContent_MakeAvailable(cexception)
  add_library(CException__impl ${cexception_SOURCE_DIR}/lib/CException.c)
  add_library(CException__hdrs INTERFACE)
  target_include_directories(CException__hdrs
                             INTERFACE ${cexception_SOURCE_DIR}/lib/)
  add_library(CException INTERFACE)
  target_link_libraries(CException INTERFACE CException__hdrs CException__impl)
endfunction()

function(add_unity)
  FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
    OVERRIDE_FIND_PACKAGE)
  FetchContent_MakeAvailable(unity)
  find_package(unity)
endfunction()

function(add_runtime_c)
  FetchContent_Declare(
    elastic_ai_runtime_c
    GIT_REPOSITORY https://github.com/es-ude/elastic-ai.runtime.c.git
    GIT_TAG v2.7.2)
  FetchContent_MakeAvailable(elastic_ai_runtime_c)
  if(NOT topicMatcher__hdrs)
    add_library(topicMatcher__hdrs INTERFACE)
    target_include_directories(
      topicMatcher__hdrs
      INTERFACE $<TARGET_PROPERTY:topicMatcher,INTERFACE_INCLUDE_DIRECTORIES>)
  endif()

  if(NOT protocol__hdrs)
    add_library(protocol__hdrs INTERFACE)
    target_include_directories(
      protocol__hdrs
      INTERFACE $<TARGET_PROPERTY:protocol,INTERFACE_INCLUDE_DIRECTORIES>)
  endif()

  function(make_impl target)
    if(NOT TARGET ${target}__impl)
      add_library(${target}__impl INTERFACE)
      target_link_libraries(${target}__impl INTERFACE ${target})
    endif()
  endfunction()

  make_impl(protocol)
  make_impl(topicMatcher)
  add_library(RuntimeC::Protocol ALIAS protocol__impl)
  add_library(RuntimeC::Protocol__impl ALIAS protocol__impl)
  add_library(RuntimeC::Protocol__hdrs ALIAS protocol__hdrs)
  add_library(RuntimeC::TopicMatcher ALIAS topicMatcher__impl)
  add_library(RuntimeC::TopicMatcher__hdrs ALIAS topicMatcher__hdrs)
  add_library(RuntimeC::TopicMatcher__impl ALIAS topicMatcher__impl)
endfunction()

function(add_freertos)
  FetchContent_Declare(
    freertos_kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG V11.0.1
    SOURCE_SUBDIR portable/ThirdParty/GCC/RP2040)
  FetchContent_MakeAvailable(freertos_kernel)

  add_library(__freertos_kernel__hdrs INTERFACE)
  target_include_directories(
    __freertos_kernel__hdrs
    INTERFACE $<TARGET_PROPERTY:FreeRTOS-Kernel,INTERFACE_INCLUDE_DIRECTORIES>)
  add_library(freeRtos::FreeRTOS-Kernel__hdrs ALIAS __freertos_kernel__hdrs)

  add_library(freeRtos::FreeRTOS-Kernel ALIAS FreeRTOS-Kernel)
  add_library(freeRtos::FreeRTOS-Kernel__impl ALIAS FreeRTOS-Kernel)

  add_library(__freertos_kernel_heap__hdrs INTERFACE)
  target_include_directories(
    __freertos_kernel_heap__hdrs
    INTERFACE
      $<TARGET_PROPERTY:FreeRTOS-Kernel-Heap3,INTERFACE_INCLUDE_DIRECTORIES>)
  add_library(freeRtos::FreeRTOS-Kernel-Heap3__hdrs ALIAS
              __freertos_kernel_heap__hdrs)

  add_library(freeRtos::FreeRTOS-Kernel-Heap3 ALIAS FreeRTOS-Kernel-Heap3)
  add_library(freeRtos::FreeRTOS-Kernel-Heap3__impl ALIAS FreeRTOS-Kernel-Heap3)
endfunction()
