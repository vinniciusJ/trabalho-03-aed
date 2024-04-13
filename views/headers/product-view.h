//
// Created by gedt on 11/04/2024.
//

#ifndef TRABALHO_3_PRODUCT_VIEW_H
#define TRABALHO_3_PRODUCT_VIEW_H

#include "../../models/product.h"
#include "../../utils/headers/utils.h"

Product * input_product();
void show_product(Product * product);
void show_products_header();
double input_new_price(int * key);
int input_new_quantity(int * key);
int input_code();
void input_string(char * label, char * value);


#endif //TRABALHO_3_PRODUCT_VIEW_H
