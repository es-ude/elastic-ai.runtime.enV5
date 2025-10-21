add_library(Add
        Add.c
)
target_include_directories(Add PUBLIC include)
target_link_libraries(Add PRIVATE
        Common
        Tensor
        DTypes
)

add_library(Arithmetic
        Arithmetic.c
)
target_include_directories(Arithmetic PUBLIC include)
target_link_libraries(Arithmetic PRIVATE
        Common
        Tensor
        DTypes
        Add
        Div
        Matmul
        Mul
        Square
        Sub
)

add_library(Comparison
        Comparison.c
)
target_include_directories(Comparison PUBLIC include)
target_link_libraries(Comparison PRIVATE
        Common
        Tensor
        DTypes
)

add_library(Div
        Div.c
)
target_include_directories(Div PUBLIC include)
target_link_libraries(Div PRIVATE
        Common
        Tensor
        DTypes
)

add_library(Matmul
        Matmul.c
)
target_include_directories(Matmul PUBLIC include)
target_link_libraries(Matmul PRIVATE
        Common
        Tensor
        DTypes
)

add_library(Mul
        Mul.c
)
target_include_directories(Mul PUBLIC include)
target_link_libraries(Mul PRIVATE
        Common
        Tensor
        DTypes
)

add_library(Square
        Square.c
)
target_include_directories(Square PUBLIC include)
target_link_libraries(Square PRIVATE
        Common
        Tensor
        DTypes
)

add_library(Sub
        Sub.c
)
target_include_directories(Sub PUBLIC include)
target_link_libraries(Sub PRIVATE
        Common
        Tensor
        DTypes
)


