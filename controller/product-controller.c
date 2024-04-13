//
// Created by gedt on 09/04/2024.
//
#include <stdio.h>

#include "headers/product-controller.h"

#include "../service/headers/product-service.h"
#include "../utils/headers/file.h"
#include "../utils/headers/b-tree.h"
#include "../models/headers.h"
#include "../views/headers/product-view.h"

void insert_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    Product * product = input_product();

    insert(product->code, product, data_file, index_file);

    free_space(product);

    fclose(index_file);
    fclose(data_file);
}

void show_products_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    IndexHeader * index_header = read_header(sizeof(IndexHeader), index_file);

    show_products_header();
    print_in_order(index_header->root, data_file, index_file);

    free_space(index_header);

    fclose(index_file);
    fclose(data_file);
}

void update_price_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    int key = 0;

    double price = input_new_price(&key);

    update_product_price(key, price, data_file, index_file);

    fclose(index_file);
    fclose(data_file);
}

void update_quantity_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    int key = 0;

    int quantity = input_new_quantity(&key);

    update_product_quantity(key, quantity, data_file, index_file);

    fclose(index_file);
    fclose(data_file);
}

void show_product_by_code_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    int code = input_code();

    Product * product = get_product_by_code(code, data_file, index_file);

    if(product != NULL){
        show_products_header();
        show_product(product);
    }

    fclose(index_file);
    fclose(data_file);
}

void show_products_codes_route(){
    FILE * index_file = open_index_file("product-index.bin");
    IndexHeader * index_header = read_header(sizeof(IndexHeader), index_file);

    show_products_code(index_header->root, index_file);

    fclose(index_file);
}