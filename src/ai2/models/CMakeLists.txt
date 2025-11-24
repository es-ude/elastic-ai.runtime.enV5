add_library(Sequential Sequential.c)
target_include_directories(Sequential PUBLIC include)
target_link_libraries(Sequential PRIVATE
        Tensor
        Layer
        Rounding
        MSE
)

