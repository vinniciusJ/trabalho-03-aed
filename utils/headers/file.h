//
// Created by vinniciusj on 31/03/24.
//

#include <stdio.h>

#ifndef TRABALHO_3_FILE_H
#define TRABALHO_3_FILE_H

// Abre um arquivo binário de índices
// Pré-condição: nome válido de arquivo e o modo para abertura
// Pós-condição: retorna um arquivo de índices aberto
FILE * open_index_file(char * filename);

// Abre um arquivo binário de dados satelites
// Pré-condição: nenhuma
// Pós-condição: retorna o arquivo aberto para leitura
FILE * open_data_file(char * filename);

#endif //TRABALHO_3_FILE_H
