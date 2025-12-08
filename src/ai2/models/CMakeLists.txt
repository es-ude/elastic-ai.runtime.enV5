add_library(ModelAPI ModelAPI.c)
target_include_directories(ModelAPI PUBLIC include)
target_link_libraries(ModelAPI PRIVATE
        Tensor
        Layer
        Rounding
        MSE
)

