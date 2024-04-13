//
// Created by gedt on 06/04/2024.
//

#include "../models/product.h"
#include "../utils/headers/utils.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef TRABALHO_3_PRODUCT_SERVICE_H
#define TRABALHO_3_PRODUCT_SERVICE_H

int split(int position, int *key, int * data_position, FILE *file);


void insert(int key, Product * product, FILE * data_file, FILE * index_file);

int insert_at_data_file(Product * product, FILE * data_file);

void print_in_order(int position, FILE * data_file, FILE * index_file);

void update_product_price(int key, double price, FILE * data_file, FILE * index_file);
void update_product_quantity(int key, int quantity, FILE * data_file, FILE * index_file);
ProductNode * search(int key, int * position, int root_position, FILE * index_file);
Product * get_product_by_code(int code, FILE * data_file, FILE * index_file);
void show_products_code(int root_position, FILE * index_file);

void execute_batch_operations(FILE * data, FILE * data_file, FILE * index_file);


#endif //TRABALHO_3_PRODUCT_SERVICE_H
