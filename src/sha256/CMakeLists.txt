add_library(binary_arithmetic_abstraction INTERFACE)
target_sources(binary_arithmetic_abstraction INTERFACE ${CMAKE_CURRENT_LIST_DIR}/BinaryArithmeticAbstraction.c)
target_include_directories(binary_arithmetic_abstraction INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_library(sha256 INTERFACE)
target_sources(sha256 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Sha256.c)
target_include_directories(sha256 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
