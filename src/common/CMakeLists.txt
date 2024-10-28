## provides helper macros to improve
## - print output

add_library(Common__hdrs INTERFACE)
target_include_directories(Common__hdrs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
add_library(Common INTERFACE)
target_link_libraries(Common INTERFACE Common__hdrs)
