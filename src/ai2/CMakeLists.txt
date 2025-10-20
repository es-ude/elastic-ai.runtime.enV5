add_subdirectory(dtypes)
add_subdirectory(tensor)
add_subdirectory(tensor_math)

add_library(ai2
        ai.c
)

target_include_directories(ai2 PUBLIC include)
target_link_libraries(ai2 PRIVATE
        Common
        DTypes
        Tensor
        TensorMath
)