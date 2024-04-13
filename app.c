//
// Created by gedt on 06/04/2024.
//
#include <stdio.h>

#include "app.h"
#include "views/headers/utils.h"
#include "controller/headers/product-controller.h"

// Imprime o menu de opções no terminal
// Pré-condição: nenhuma
// Pós-condição: o menu é impresso no terminal
void show_menu(){
    printf("+-----------SISTEMA DE GERENCIAMENTO DE PRODUTOS (SGP)-----------+\n");
    printf("|[0] - Sair do sistema                                           |\n");
    printf("|[1] - Cadastrar produto                                         |\n");
    printf("|[2] - Remover produto                                           |\n");
    printf("|[3] - Mostrar todos os produtos                                 |\n");
    printf("|[4] - Atualizar estoque de um produto                           |\n");
    printf("|[5] - Atualizar preco de um produto                             |\n");
    printf("|[6] - Mostrar produto por codigo                                |\n");
    printf("|[7] - Mostrar a arvore de indices dos produtos                  |\n");
    printf("|[8] - Mostrar posicoes livres no arquivo de indices             |\n");
    printf("|[9] - Mostrar posicoes livres no arquivo de dados               |\n");
    printf("|[10] - Realizar operacoes em lote                               |\n");
    printf("+----------------------------------------------------------------+\n");
}

// Inicia a navegação do sistema e redireciona para o recurso solicitado
// Pré-condição: nenhuma
// Pós-condição: redireciona o usuário para o recurso solicitado
void start_app_router(){
    int option = 0;

    show_menu();

    option = input_option();

    switch (option) {
        case 1:
            insert_route();
            break;
        case 3:
            show_products_route();
            break;
        case 4:
            update_quantity_route();
            break;
        case 5:
            update_price_route();
            break;
        case 6:
            show_product_by_code_route();
            break;
        case 7:
            show_products_codes_route();
            break;
        default:
            start_app_router();
    }

    start_app_router();

}

