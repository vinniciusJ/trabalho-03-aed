//
// Created by vinniciusj on 31/03/24.
//
#include "headers/b-tree.h"
#include "headers/file.h"

// Abre um arquivo binário de índices
// Pré-condição: nome válido de arquivo e o modo para abertura
// Pós-condição: retorna um arquivo de índices aberto
FILE * open_index_file(char * filename){
    FILE * file_exists = fopen(filename, "rb");

    if(file_exists == NULL){
        fclose(file_exists);
        FILE * file = fopen(filename, "w+b");

        init_index_file(file);

        return file;
    }

    fclose(file_exists);

    return fopen(filename, "r+b");
}

// Abre um arquivo binário de dados satelites
// Pré-condição: nenhuma
// Pós-condição: retorna o arquivo aberto para leitura
FILE * open_data_file(char * filename){
    FILE * file_exists = fopen(filename, "rb");

    if(file_exists == NULL){
        fclose(file_exists);
        FILE * file = fopen(filename, "w+b");

        init_data_file(file);

        return file;
    }

    fclose(file_exists);

    return fopen(filename, "r+b");
}

// Abre um arquivo binário de dados satelites
// Pré-condição: nenhuma
// Pós-condição: retorna o arquivo aberto para leitura
FILE * open_txt_file(char * filename) {
    FILE * file;
    file = fopen(filename, "r");

    if(file == NULL) {
        printf("\nNão foi possível abrir o arquivo!\n");
        return NULL;
    }

    return file;
}