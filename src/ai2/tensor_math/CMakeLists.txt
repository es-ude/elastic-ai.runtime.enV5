add_library(add
        add.c
)
target_include_directories(add PUBLIC include)
target_link_libraries(add PRIVATE
        Common
        Tensor
        DTypes
)


add_library(TensorMath
        TensorMath.c
)
target_include_directories(TensorMath PUBLIC include)
target_link_libraries(TensorMath PRIVATE
        Common
        Tensor
        DTypes
        add
)

