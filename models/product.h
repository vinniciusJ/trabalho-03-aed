//
// Created by vinniciusj on 31/03/24.
//

#ifndef TRABALHO_3_PRODUCT_H
#define TRABALHO_3_PRODUCT_H

#define MAX_NAME 50
#define MAX_BRAND 30
#define MAX_CATEGORY 50
#define ORDER 5

#define MIN_KEYS (ORDER - 1) / 2
#define MAX_KEYS 4

typedef struct {
    int code;
    char name[MAX_NAME + 1];
    char brand[MAX_BRAND + 1];
    char category[MAX_CATEGORY + 1];
    int quantity;
    double price;
} Product;

typedef struct {
    int keys[ORDER];
    int children[ORDER+1];
    int data[ORDER];
    int keys_length;
    int is_leaf;
} ProductNode;

#endif //TRABALHO_3_PRODUCT_H
