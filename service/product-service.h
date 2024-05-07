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
int search_position_in_node(int key, int *position, int node_pos, FILE *file);

// Busca um a posição de um nó no aquivo de indices
// Pré-condição: chave a ser procurada e
// Pós-condição: retorna a posição do nó no arquivo de indices
int search_node(int key, FILE * file);

// Função auxiliar para bsucar a posição de um nó no arquivo de indices
// Pré-condição: chave a ser procurada e
// Pós-condição: retorna a posição do nó no arquivo de indices
int search_node_aux(int key, int position, FILE * file);

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

// Atualiza a lista de posições livres no arquivo de dados após a remoção de um registro
// Pré-condição: posição do nó livre e arquivo aberto a escrita
// Pós-condição: posição adicionada a "lista" de posições livres
void update_free_positions_data(int position, FILE * data_file);

// Função para remover um produto
// Pré-condição: código do produto e arquivos abertos para escrita
// Pós-condição: produto removido dos arquivos de dados e indices
void remove_product(int key, FILE * index_file, FILE * data_file);

// Atualiza um nó folha recém removido
// Pré-condição: nó-folha removido, posição no arquivo e arquivos abertos p/ leitura e escrita
// Pós-condição: nó-folha atualizado
void update_removed_leaf_node(ProductNode * leaf, int remove_pos, int remove_pos_in_node, FILE * index_file, FILE * data_file);

// Representa o caso 1 de remoção: a chave está num nó folha cujo número de chaves é maior que o mínimo
// Pré-condição: chave, nó a ser removido, a posição do nó e arquivos abertos para escrita
// Pós-condição: chave removida do nó folha
void remove_case1(ProductNode * remove_node, int key, int remove_pos, FILE * index_file, FILE * data_file);

// Verifica se o no tem mais chaves do que o mínimo
// Pré-condição: um nó não-nulo
// Pós-condição: retorno da verificação
int has_more_than_min_keys(ProductNode * node);

// Busca a chave sucessora de um nó folha
// Pré-condição: nó a ser removido e ponteiro para armazenar a próxima posição
// Pós-condição: chave sucessora do nó removido
int search_next_key_leaf(ProductNode * remove_node, int key_pos, int * next_node_pos, FILE *index_file);

// Função para buscar o pai de um nó na árvore B a partir de um código de produto
// Pré-condição: chave a ser pesquisada, posição inicial e arquivo aberto para leitura
// Pós-condição: posição do nó pai
int search_father(int key, FILE * index_file);

// Função auxiliar para buscar o pai de um nó na árvore B a partir de um código de produto
// Pré-condição: chave a ser pesquisada, posição inicial e arquivo aberto para leitura
// Pós-condição: posição do nó pai
int search_father_aux(int key, int root_pos, FILE * index_file);

// Busca a posição de um filho em um nó
// Pré-condição: nó não nulo e a posição do filho
// Pós-condição: posição do filho
int search_son_pos(ProductNode * node, int pos);

// Representa o caso 2 da remoção: a remoção de chave que está em um nó interno
// Pré-condição: chave a ser removida, posição do nó e arquivos abertos para escrita
// Pré-condição: produto removido com sucesso dos arquivos
int remove_case2(int key, int remove_pos, ProductNode * remove_node, FILE *index_file, FILE * data_file);

// Verifica se é possível realizar a redistribuição de chaves entre irmãos
// Pré-condição: posição do nó pai e ponteiros para armazenar para onde ocorrerá a redistribuição
// Pós-condição: retorna da verificação
int can_redistribute(int father_pos, int son_index, int * left_pos, int * right_pos, FILE *index_file);

// Redistribui chaves a partir do irmão direito
// Pré-condição: posição do nó pai e arquivos aberto para escrita
// Pós-condição: redistribuição a partir do irmãp a direita
void redistribute_right(int father_pos, int remove_pos, int remove_son_pos, int right_pos,  FILE *index_file);

// Redistribui chaves a partir do irmão esquerdo
// Pré-condição: posição do nó pai e arquivos aberto para escrita
// Pós-condição: redistribuição a partir do irmãp a esquerda
void redistribute_left(int father_pos, int remove_pos, int remove_son_pos, int left_pos, FILE *index_file);

// Realiza a redistribuição de chaves entre irmãos
// Pré-condição: posição do pai para fazer a redistribuição, posiçoes de orientação e arquivos abertos para escrita
// Pós-condição: redistribuição das chaves
void redistribute(int father_pos, int remove_pos, int remove_son_pos, int left_pos, int right_pos,  FILE *index_file);

// Busca os filhos esquerdo e direito de um nó pai
// Pré-condição: lorem ipsum dolor sit amet
// Pós-condição: posição dos filhos a esquerda e a direita
void search_leaft_right_children(int father_pos, int remove_pos, int * left_pos, int * right_pos,  FILE *index_file);

// Concatena o nó da esquerda com o nó a ser removido
// Pré-condição: nó-pai não-nulo e nó a ser removido, além do arquivo aberto para escrita
// Pós-condição: concatenação a esquerda
void combine_left(ProductNode * father, ProductNode * left, ProductNode * remove_node, int son_index, FILE * index_file);

// Concatena o nó da direita com o nó a ser removido
// Pré-condição: nó-pai não-nulo e nó a ser removido, além do arquivo aberto para escrita
// Pós-condição: concatenação a direita
void combine_right(ProductNode * father, ProductNode * right, ProductNode * remove_node, int son_index, FILE * index_file);

// Concatena nós para manter propriedades da arvore B
// Pré-condição: posições dos pai, do nó a ser removido e arquivo aberto para escrita
// Pós-condição: nós concatenados
void combine(int father_pos, int remove_pos, int son_index, int left_pos, int right_pos, FILE * index_file);

// Balancea a árvore após uma remoção
// Pré-condição: posição do nó pai e arquivo aberto para escrita
// Pós-condição: nó pai balanceado
void balance(int father_pos, int son_index, int remove_pos,  FILE *index_file);

// Verifica e atualiza um pai após uma remoção
// Pré-condição: posição do nó-pai e arquivo aberto para escrita
// Pós-condição: nó pai estabilizado
void stabilize_father(int position, FILE *index_file);

// Função auxiliar para a remoção de uma chave
// Pré-condição: chave, posiççoes de referência e arquivos abertos para escrita
// Pós-condição: chave removida
void remove_key(int key, int root_pos, int remove_pos, FILE *index_file, FILE * data_file);

// Mostrar as posições livres do arquivo de indices
// Pré-condição: arquivo aberto para leitura
// Pós-condição: lista de posições livres impressas no terminal
void show_free_positions_from_index_file(FILE *index_file);

// Mostrar as posições livres do arquivo de indices
// Pré-condição: arquivo aberto para leitura
// Pós-condição: lista de posições livres impressas no terminal
void show_free_positions_from_data_file(FILE *data_file);

#endif //TRABALHO_3_PRODUCT_SERVICE_H
