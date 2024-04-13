//
// Created by gedt on 06/04/2024.
//

#include <stdio.h>

#ifndef TRABALHO_3_UTILS_H
#define TRABALHO_3_UTILS_H

// Escaneia um valor inteiro pelo terminal informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o valor digitado pelo usuário
int input_option() {
    int option = 0;

    printf("Escolha uma opcao das listadas acima: ");
    scanf("%d%*c", &option);
    printf("\n");

    return option;
}

#endif //TRABALHO_3_UTILS_H
