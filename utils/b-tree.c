//
// Created by vinniciusj on 31/03/24.
//
#include "headers/utils.h"
#include "headers/b-tree.h"

// Lê uma nó da lista em uma determinada posição do arquivo
// Pré-condição: arquivo deve estar aberto e ser um arquivo de lista e a posição deve ser uma posição válida da lista
// Pós-condição: ponteiro para nó lido é retornado
void * read_node(int position, size_t size, size_t header_size, FILE * file){
    if(position == -1){
        return NULL;
    }

    void * item = alloc(size);

    fseek(file, header_size + position * size, SEEK_SET);
    fread(item, size, 1, file);

    return item;
}

// Escreve um nó em uma determinada posição do arquivo
// Pré-condição: arquivo deve estar aberto e ser um arquivo de lista, deve passar o tamanho do nó e a posição deve ser valida no arquivo
// Pós-condição: nó escrito no arquivo
void set_node(void * node, size_t size, size_t header_size, int position, FILE * file){
    fseek(file, header_size + position * size, SEEK_SET);
    fwrite(node, size, 1, file);
}

// Criar uma lista nova em um arquivo
// Pré-condição: arquivo aberto para leitura/escrita
// Pós-condição: arquivo é inicializado com uma lista vazia
void init_index_file(FILE * file){
    IndexHeader * header = (IndexHeader *) alloc(sizeof(IndexHeader));

    header->root = -1;
    header->top = 0;
    header->free = -1;

    set_header(header, sizeof(IndexHeader), file);

    free_space(header);
}

// Criar uma lista nova em um arquivo
// Pré-condição: arquivo aberto para leitura/escrita
// Pós-condição: arquivo é inicializado com uma lista vazia
void init_data_file(FILE * file){
    DataHeader * header = (DataHeader *) alloc(sizeof(DataHeader));

    header->top = 0;
    header->free = -1;

    set_header(header, sizeof(DataHeader), file);

    free_space(header);
}

// Lê o cabeçalho do arquivo contendo as informações da lista
// Pré-condição: arquivo deve estar aberto e deve ser um arquivo de lista
// Pós-condição: retorna o ponteiro para o cabeçalho lido
void * read_header(size_t header_size, FILE * file){
    void * header = alloc(header_size);

    fseek(file, 0, SEEK_SET);
    fread(header, header_size, 1, file);

    return header;
}

// Escreve no arquivo o cabeç alho contendo as informações da lista
// Pré-condição: arquivo deve estar aberto e ser um arquivo de lista
// Pós-condição: cabeçaalho escrito no arquivo
void set_header(void * header, size_t header_size, FILE * file){
    fseek(file, 0, SEEK_SET);
    fwrite(header, header_size, 1, file);
}

// Verifica se a lista está vazia
// Pré-condição: o cabeçalho do arquivo
// Pós-condição: retorna se a lista está vazia
int is_index_file_empty(IndexHeader * header){
    return header->root == -1;
}

// Verifica se a lista está vazia
// Pré-condição: o cabeçalho do arquivo
// Pós-condição: retorna se a lista está vazia
int is_data_file_header(IndexHeader * header){
    return header->top == 0;
}

// Verifica se uma determinada posição guarda a raiz árvore
int is_root(int position, FILE * file){
    IndexHeader *header = read_header(sizeof(IndexHeader), file);

    return position == header->root;
}