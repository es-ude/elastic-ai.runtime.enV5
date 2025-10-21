add_library(add
        add.c
)
target_include_directories(add PUBLIC include)
target_link_libraries(add PRIVATE
        Common
        Tensor
        DTypes
)

add_library(div
        div.c
)
target_include_directories(div PUBLIC include)
target_link_libraries(div PRIVATE
        Common
        Tensor
        DTypes
)

add_library(matmul
        matmul.c
)
target_include_directories(matmul PUBLIC include)
target_link_libraries(matmul PRIVATE
        Common
        Tensor
        DTypes
)

add_library(mul
        mul.c
)
target_include_directories(mul PUBLIC include)
target_link_libraries(mul PRIVATE
        Common
        Tensor
        DTypes
)

add_library(square
        square.c
)
target_include_directories(square PUBLIC include)
target_link_libraries(square PRIVATE
        Common
        Tensor
        DTypes
)

add_library(sub
        sub.c
)
target_include_directories(sub PUBLIC include)
target_link_libraries(sub PRIVATE
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
        add
        div
        matmul
        mul
        square
        sub
)

