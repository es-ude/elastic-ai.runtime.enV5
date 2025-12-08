add_subdirectory(tensor)
add_subdirectory(arithmetic)
add_subdirectory(layer)
add_subdirectory(models)
add_subdirectory(loss_functions)
add_subdirectory(optimizer)
add_subdirectory(csv)

add_library(ai2
        ai.c
)

target_include_directories(ai2 PUBLIC include)
target_link_libraries(ai2 PRIVATE
        Common
        DTypes
        Tensor
        Arithmetic
        Quantization
        Layer
)