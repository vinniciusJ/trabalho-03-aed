//
// Created by gedt on 09/04/2024.
//
#include <stdio.h>

#include "product-controller.h"

#include "../service/product-service.h"
#include "../utils/headers/file.h"
#include "../utils/headers/b-tree.h"
#include "../models/headers.h"
#include "../views/headers/product-view.h"

// Rota para fazer a inserção de um produto
// Pré-condição: nenhuma
// Pós-condição: produto inserido no arquivo de índices e de dados
void insert_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    Product * product = input_product();

    insert(product->code, product, data_file, index_file);

    free_space(product);

    fclose(data_file);

    fclose(index_file);
}

// Rota para mostrar todos os produtos ordenados pelo código
// Pré-condição: nenhuma
// Pós-condição: todos os produtos são mostrados no terminal
void show_products_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    IndexHeader * index_header = read_header(sizeof(IndexHeader), index_file);

    show_products_header();
    show_products(index_header->root, data_file, index_file);

    free_space(index_header);

    fclose(index_file);
    fclose(data_file);
}

// Rota para atualizar o preço de um produto
// Pré-condição: nenhuma
// Pós-condição: preço do produto atualizado
void update_price_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    int key = 0;

    double price = input_new_price(&key);

    update_product_price(key, price, data_file, index_file);

    fclose(index_file);
    fclose(data_file);
}

// Rota para atualizar o estoque de um produto
// Pré-condição: nenhuma
// Pós-condição: estoque do produto atualizado
void update_quantity_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    int key = 0;

    int quantity = input_new_quantity(&key);

    update_product_quantity(key, quantity, data_file, index_file);

    fclose(index_file);
    fclose(data_file);
}

// Rota para mostrar os dados de um produto pelo código
// Pré-condição: nenhuma
// Pós-condição: produto é mostrado no terminal
void show_product_by_code_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    int code = input_code();

    Product * product = get_product_by_code(code, data_file, index_file);

    if(product != NULL){
        show_products_header();
        show_product(product);
    } else {
        show_not_found_message();
    }

    fclose(index_file);
    fclose(data_file);
}

// Rota para a mostrar a árvore B formada pelos códigos
// Pré-condição: nenhuma
// Pós-condição: árvore B dos códigos é mostrada no terminal
void show_products_codes_route(){
    FILE * index_file = open_index_file("product-index.bin");
    IndexHeader * index_header = read_header(sizeof(IndexHeader), index_file);

    show_products_code(index_header->root, index_file);

    fclose(index_file);
}

// Rota para executar as operações em lote
// Pré-condição: nenhuma
// Pós-condição: operações em lote executadas
void execute_batch_operations_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    char filename[50];

    input_string("Digite o nome do arquivo: ", filename);

    FILE * data = open_txt_file(filename);

    if(data == NULL){
        return;
    }

    execute_batch_operations(data, data_file, index_file);

    fclose(data_file);
    fclose(data);
    fclose(index_file);
}

void remove_product_route(){
    FILE * index_file = open_index_file("product-index.bin");
    FILE * data_file = open_data_file("product-data.bin");

    int code = input_code();
    remove_product(code, index_file, data_file);

    fclose(index_file);
    fclose(data_file);
}

void show_free_positions_from_index_file_route(){
    FILE * index_file = open_index_file("product-index.bin");

    show_free_positions_from_index_file(index_file);

    fclose(index_file);
}

void show_free_positions_from_data_file_route(){
    FILE * data_file = open_data_file("product-data.bin");

    show_free_positions_from_data_file(data_file);

    fclose(data_file);
}