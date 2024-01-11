## provides helper macros to improve
## - print output

add_library(common_lib INTERFACE)
target_include_directories(common_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
