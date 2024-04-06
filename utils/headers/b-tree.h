//
// Created by vinniciusj on 31/03/24.
//
#include <stdlib.h>
#include <stdio.h>
#include "../../models/headers.h"

#ifndef TRABALHO_3_B_TREE_H
#define TRABALHO_3_B_TREE_H

// Lê uma nó da lista em uma determinada posição do arquivo
// Pré-condição: arquivo deve estar aberto e ser um arquivo de lista e a posição deve ser uma posição válida da lista
// Pós-condição: ponteiro para nó lido é retornado
void * read_node(int position, size_t size, size_t header_size, FILE * file);

// Escreve um nó em uma determinada posição do arquivo
// Pré-condição: arquivo deve estar aberto e ser um arquivo de lista, deve passar o tamanho do nó e a posição deve ser valida no arquivo
// Pós-condição: nó escrito no arquivo
void set_node(void * node, size_t size, size_t header_size, int position, FILE * file);

// Criar uma lista nova em um arquivo
// Pré-condição: arquivo aberto para leitura/escrita
// Pós-condição: arquivo é inicializado com uma lista vazia
void init_index_file(FILE * file);

// Criar uma lista nova em um arquivo
// Pré-condição: arquivo aberto para leitura/escrita
// Pós-condição: arquivo é inicializado com uma lista vazia
void init_data_file(FILE * file);

// Lê o cabeçalho do arquivo contendo as informações da lista
// Pré-condição: arquivo deve estar aberto e deve ser um arquivo de lista
// Pós-condição: retorna o ponteiro para o cabeçalho lido
void * read_header(size_t header_size, FILE * file);

// Escreve no arquivo o cabeç alho contendo as informações da lista
// Pré-condição: arquivo deve estar aberto e ser um arquivo de lista
// Pós-condição: cabeçaalho escrito no arquivo
void set_header(void * header, size_t header_size, FILE * file);

// Verifica se a lista está vazia
// Pré-condição: o cabeçalho do arquivo
// Pós-condição: retorna se a lista está vazia
int is_index_file_empty(IndexHeader * header);

// Verifica se a lista está vazia
// Pré-condição: o cabeçalho do arquivo
// Pós-condição: retorna se a lista está vazia
int is_data_file_header(IndexHeader * header);

#endif //TRABALHO_3_B_TREE_H
