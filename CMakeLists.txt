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
        utils/b-tree.c
        service/product-service.h
        service/product-service.c
        service/product-service.c
        app.h
        app.c
        views/headers/utils.h
        controller/product-controller.h
        controller/product-controller.c
        views/headers/product-view.h
        views/product-view.c
        utils/headers/queue.h
        utils/queue.c
)
