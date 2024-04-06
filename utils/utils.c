//
// Created by vinniciusj on 31/03/24.
//
#include <stdio.h>

#include "headers/utils.h"

// Aloca um espaço dinâmico na memória
// Pŕe-condição: tamanho do espaço a ser alocado na memória
// Pós-condição: retorna um ponteiro referenciado ao enedreço alocado
void * alloc(size_t size){
    void * ptr = malloc(size);

    if(ptr == NULL){
        printf("Não há espaço em memória\n");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

// Libera um espaço alocado dinamicamente na memória
// Pré-condição: o endereço alocado dinamicamente
// Pós-condição: retorna um endereço NULL
void * free_space(void * ptr){
    free(ptr);

    return NULL;
}
