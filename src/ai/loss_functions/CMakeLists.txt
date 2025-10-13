add_library(MSE
        MSE.c
)
target_include_directories(MSE PUBLIC include)
target_link_libraries(MSE PRIVATE
        Common
        AiHelpers
        Quantization
)


add_library(CrossEntropy
        CrossEntropy.c
)
target_include_directories(CrossEntropy PUBLIC include)
target_link_libraries(CrossEntropy PRIVATE
        Common
        AiHelpers
        Softmax
        Quantization
        m
)