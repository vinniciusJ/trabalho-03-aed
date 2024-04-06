cmake_minimum_required(VERSION 3.27)
project(trabalho_3 C)

set(CMAKE_C_STANDARD 11)

add_executable(trabalho_3 main.c
        models/product.h
        models/headers.h
        utils/headers/file.h
        utils/file.c
        utils/headers/b-tree.h
        utils/headers/utils.h
        utils/utils.c
        utils/b-tree.c)
