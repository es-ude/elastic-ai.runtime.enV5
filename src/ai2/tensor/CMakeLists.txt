add_library(DTypes
        DTypes.c
)
target_include_directories(DTypes PUBLIC include)
target_link_libraries(DTypes PRIVATE
        Common
        Rounding
)

add_library(DataStorage DataStorage.c)

target_include_directories(DataStorage PUBLIC include)
target_link_libraries(DataStorage PRIVATE
        Common
)

add_library(Quantization
        Quantization.c
)
target_include_directories(Quantization PUBLIC include)
target_link_libraries(Quantization PRIVATE
        Common
        Rounding
        m
)

add_library(Tensor
        Tensor.c
)
target_include_directories(Tensor PUBLIC include)
target_link_libraries(Tensor PRIVATE
        Common
        DTypes
        DataStorage
        Rounding
)

