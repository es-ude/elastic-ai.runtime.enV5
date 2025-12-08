add_library(CrossEntropy
        CrossEntropy.c)
target_include_directories(CrossEntropy PUBLIC include)
target_link_libraries(CrossEntropy PRIVATE
        Tensor
        CommonLayerLibs
)

add_library(MSE
        MSE.c
)
target_include_directories(MSE PUBLIC include)
target_link_libraries(MSE PRIVATE
        Tensor
        TensorConversion
        Square
        Sub
)