set(MIDDLEWARE_STUB_VERSION "1" CACHE STRING
  "use stub version 1, compatible with skeleton version 1"
)
set(MIDDLEWARE_STUB_VERSION "2" CACHE STRING
  "use stub version 2, compatible with skeleton version 2"
)

add_library(Stub stub.c)
target_include_directories(Stub PUBLIC include include_v2)
target_link_libraries(Stub Common Sleep Middleware)
