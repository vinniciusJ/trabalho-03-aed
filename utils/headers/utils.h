//
// Created by vinniciusj on 31/03/24.
//
#include <stdlib.h>

#ifndef TRABALHO_3_UTILS_H
#define TRABALHO_3_UTILS_H

// Aloca um espaço dinâmico na memória
// Pŕe-condição: tamanho do espaço a ser alocado na memória
// Pós-condição: retorna um ponteiro referenciado ao enedreço alocado
void * alloc(size_t size);

// Libera um espaço alocado dinamicamente na memória
// Pré-condição: o endereço alocado dinamicamente
// Pós-condição: retorna um endereço NULL
void * free_space(void * ptr);

#endif //TRABALHO_3_UTILS_H
