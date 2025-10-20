add_library(DataStorage DataStorage.c)

target_include_directories(DataStorage PUBLIC include)
target_link_libraries(DataStorage PRIVATE
        Common
)

add_library(Tensor
        Tensor.c
)
target_include_directories(Tensor PUBLIC include)
target_link_libraries(Tensor PRIVATE
        Common
        DTypes
        DataStorage
)

