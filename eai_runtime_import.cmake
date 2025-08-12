# This is a copy of <ELASTIC_AI_RUNTIME_PATH>/eai_runtime_import.cmake

# This can be dropped into an external project
# to help locate the elastic-ai runtime
# It should be include()ed prior to project()

if(DEFINED ENV{EAI_RUNTIME_PATH} AND(NOT EAI_RUNTIME_PATH))
  set(EAI_RUNTIME_PATH $ENV{EAI_RUNTIME_PATH})
  message("Using EAI_RUNTIME_PATH from environment('${EAI_RUNTIME_PATH}')")
endif()

if(DEFINED ENV{EAI_RUNTIME_FETCH_FROM_GIT} AND(NOT EAI_RUNTIME_FETCH_FROM_GIT))
  set(EAI_RUNTIME_FETCH_FROM_GIT $ENV{EAI_RUNTIME_FETCH_FROM_GIT})
  message("Using EAI_RUNTIME_FETCH_FROM_GIT from environment('${EAI_RUNTIME_FETCH_FROM_GIT}')")
endif()

if(DEFINED ENV{EAI_RUNTIME_FETCH_FROM_GIT_PATH} AND (NOT EAI_RUNTIME_FETCH_FROM_GIT_PATH))
  set(EAI_RUNTIME_FETCH_FROM_GIT_PATH $ENV{EAI_RUNTIME_FETCH_FROM_GIT_PATH})
  message("Using EAI_RUNTIME_FETCH_FROM_GIT_PATH from environment('${EAI_RUNTIME_FETCH_FROM_GIT_PATH}')")
endif()

if(DEFINED ENV{EAI_RUNTIME_FETCH_FROM_GIT_TAG} AND (NOT EAI_RUNTIME_FETCH_FROM_GIT_TAG))
  set(EAI_RUNTIME_FETCH_FROM_GIT_TAG $ENV{EAI_RUNTIME_FETCH_FROM_GIT_TAG})
  message("Using EAI_RUNTIME_FETCH_FROM_GIT_TAG from environment('${EAI_RUNTIME_FETCH_FROM_GIT_TAG}')")
endif()

if(EAI_RUNTIME_FETCH_FROM_GIT AND NOT EAI_RUNTIME_FETCH_FROM_GIT_TAG)
  set(EAI_RUNTIME_FETCH_FROM_GIT_TAG "main")
  message("Using main as default value for EAI_RUNTIME_FETCH_FROM_GIT_TAG")
endif()

set(EAI_RUNTIME_PATH "${EAI_RUNTIME_PATH}"
  CACHE
  PATH
  "Path to the EAI runtime"
)
set(EAI_RUNTIME_FETCH_FROM_GIT "${EAI_RUNTIME_FETCH_FROM_GIT}"
  CACHE
  BOOL
  "Set to ON to fetch copy of SDK from git if not otherwise locatable"
)
set(EAI_RUNTIME_FETCH_FROM_GIT_PATH "${EAI_RUNTIME_FETCH_FROM_GIT_PATH}"
  CACHE
  FILEPATH
  "location to download runtime"
)
set(EAI_RUNTIME_FETCH_FROM_GIT_TAG "${EAI_RUNTIME_FETCH_FROM_GIT_TAG}"
  CACHE
  FILEPATH
  "release tag for runtime"
)

if(NOT EAI_RUNTIME_PATH)
  if(EAI_RUNTIME_FETCH_FROM_GIT)
    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
    if(EAI_RUNTIME_FETCH_FROM_GIT_PATH)
      get_filename_component(FETCHCONTENT_BASE_DIR "${EAI_RUNTIME_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
    endif()
    # GIT_SUBMODULES_RECURSE was added in 3.17
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17.0")
      FetchContent_Declare(
        eai_runtime
        GIT_REPOSITORY https://github.com/es-ude/elastic-ai.runtime.enV5.git
        GIT_TAG ${EAI_RUNTIME_FETCH_FROM_GIT_TAG}
        GIT_SUBMODULES_RECURSE FALSE
        OVERRIDE_FIND_PACKAGE
      )
    else()
      FetchContent_Declare(
        eai_runtime
        GIT_REPOSITORY https://github.com/es-ude/elastic-ai.runtime.enV5.git
        GIT_TAG ${EAI_RUNTIME_FETCH_FROM_GIT_TAG}
        OVERRIDE_FIND_PACKAGE
      )
    endif()

    if(NOT eai_runtime)
      message("Downloading EAI runtime")
      FetchContent_MakeAvailable(eai_runtime)
      set(EAI_RUNTIME_PATH ${eai_runtime_SOURCE_DIR})
    endif()
    set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})
  else()
    message(FATAL_ERROR
      "EAI runtime location was not specified. Please set EAI_RUNTIME_PATH or set EAI_RUNTIME_FETCH_FROM_GIT to ON to fetch from git."
    )
  endif()
endif()

get_filename_component(EAI_RUNTIME_PATH
  "${EAI_RUNTIME_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}"
)
if(NOT EXISTS ${EAI_RUNTIME_PATH})
  message(FATAL_ERROR "Directory '${EAI_RUNTIME_PATH}' not found")
endif()

set(EAI_RUNTIME_INIT_CMAKE_FILE ${EAI_RUNTIME_PATH}/eai_runtime_init.cmake)
if(NOT EXISTS ${EAI_RUNTIME_INIT_CMAKE_FILE})
  message(FATAL_ERROR
    "Directory '${EAI_RUNTIME_PATH}' does not appear to contain the EAI runtime"
  )
endif()

set(EAI_RUNTIME_PATH ${EAI_RUNTIME_PATH}
  CACHE
  PATH
  "Path to the EAI runtime"
  FORCE
)

include(${EAI_RUNTIME_INIT_CMAKE_FILE})
