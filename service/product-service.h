//
// Created by gedt on 06/04/2024.
//

#include "../models/product.h"
#include "../utils/headers/utils.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef TRABALHO_3_PRODUCT_SERVICE_H
#define TRABALHO_3_PRODUCT_SERVICE_H

// Realiza o split de nós cheios
// Pré-condição: posição do nó e arquivo de índíces aberto para leitura e escrita
// Pós-condição: retorna a nova posição, a chave promovida e a posição da chave promovida
int split(int position, int *key, int * data_position, FILE *file);

// Insere um produto no arquivo de dados e retorna a posição no arquivo
// Pré-condição: um produto válido
// Pós-condição: produto inserido no arquivo de dados
int insert_at_data_file(Product * product, FILE * data_file);

// Verifica se a chave está no nó e retorna a posição dela
// Pré-condição: uma posição inicial e o arquivo de indices aberto para leitura e escrita
// Pós-condição: retorna se a chave está
int search_position(int key, int *position, int start_position, FILE *file);

// Adiciona uma chave em um nó não cheio
// Pré-condição: posição do nó, chave, posição da chave no arquivo de dados e um arquivo de índices aberto para leitura e escrita
// Pós-condição: a chave é inserida no nó
void add_node_at_right(int key_position, int key, int data_position, int child_position, int node_position, FILE *file);

// Verica se o nó está em overflow
// Pré-condição: um nó não-nulo
// Pós-condição: retorna se o nó está ou não em overflow
int is_overflow(ProductNode * node);

// Função auxiliar para inserção
// Pré-condição: posição do dados noa arquivo de dados,a posição do nó pai e um arquivo aberto para leitura e escrita
// Pós-condição: nó inserido nos arquivos
void insert_node(int key, int data_position, int node_position, FILE *file);

// Insere um produto no arquivo de indices e no de dados
// Pós-condição: produto válido e arquivos de dados e indices abertos pra escritura e escrita
// Pré-condição: produto inserido com sucesso nos dois arquivos
void insert(int key, Product * product, FILE * data_file, FILE * index_file);

// Busca um nó de um produto pela chave
// Pré-condição: ponteiro para a posição da chave no nó e arquivo aberto p/ leitura e escrita
// Pós-condição: retorna o nó do produto ou NULL
ProductNode * search(int key, int * position, int root_position, FILE * index_file);

// Busca um produto pela chave
// Pré-condição: arquivos de indices e dados abertos para leitura e escrita
// Pós-condição: retorna o produto ou NULL
Product * get_product_by_code(int code, FILE * data_file, FILE * index_file);

// Edita o preço de um produto
// Pré-condição: a chave do produto, o novo preços e os arquivos de dados e indices abertos para leitura e escrita
// Pós-condição: o preço do produto é atualizado no arquivo de dados
void update_product_price(int key, double price, FILE * data_file, FILE * index_file);

// Edita o estoque de um produto
// Pré-condição: a chave do produto, o novo estoque e os arquivos de dados e indices abertos para leitura e escrita
// Pós-condição: o estoque do produto é atualizado no arquivo de dados
void update_product_quantity(int key, int quantity, FILE * data_file, FILE * index_file);

// Imprime a arvore de produtos ordenado crescentemente pelo código
// Pre-condição: arquivos de dados e indices aberto p/ leitura e escrita
// Pós-condição: todos os produtos são listados no terminal ordenados pelo código
void show_products(int position, FILE * data_file, FILE * index_file);

// Imprime os códigos dos produtos formatados como uma arvore B
// Pre-condição: arquivos de dados e indices aberto p/ leitura e escrita
// Pós-condição: os códigos dos produtos são listados como uma arvore B no terminal
void show_products_code(int root_position, FILE * index_file);

// Executa as operações em lote
// Pré-condição: arquivo das operações, de dados e de indices abertos para escrita e leitura
// Pós-condição: todas as operações são executadas
void execute_batch_operations(FILE * data, FILE * data_file, FILE * index_file);

// Atualiza o dado de um produto a partir de uma string de operação em lote
// Pré-condição: string da operação e arquivos de indices e dados abertos para leitura e escrita
// Pós-codição: produto é atualizado com sucesso
void update_by_string(const char * string, FILE * data_file, FILE * index_file);

// Insere um produto a partir de uma string de operação em lote
// Pré-condição: string da operação e arquivos de indices e dados abertos para leitura e escrita
// Pós-codição: produto é inserido com sucesso
void insert_by_string(const char * string, FILE * data_file, FILE * index_file);

void remove_product(int code, FILE * data_file, FILE * index_file);

void merge_nodes(int position, int node_position, FILE * index_file);

void remove_key(int key, int node_pos, ProductNode * remove_node, FILE *file);

#endif //TRABALHO_3_PRODUCT_SERVICE_H
