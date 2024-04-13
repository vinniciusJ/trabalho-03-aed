//
// Created by gedt on 11/04/2024.
//

#ifndef TRABALHO_3_PRODUCT_VIEW_H
#define TRABALHO_3_PRODUCT_VIEW_H

#include "../../models/product.h"
#include "../../utils/headers/utils.h"

// Escaneia os valores de um produto pelo terminal informados pelo usuario
// Pré-condição: nenhuma
// Pós-condição: retorna um produto informado pelo usuário
Product * input_product();

// Mostra um produto formado em uma tabela
// Pré-condição: nenhuma
// Pós-condição: mostra no terminal os dados de um produto
void show_product(Product * product);

// Mostra o cabeçalho da tabela de produtos
// Pré-condição: nenhuma
// Pós-condição: mostra o cabeçalho da tabela de produtos no terminal
void show_products_header();

// Escaneia o valor de um preço informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o valor lido e a chave informado pelo usuario
double input_new_price(int * key);

// Escaneia o valor de um estoque informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o valor lido e a chave informado pelo usuario
int input_new_quantity(int * key);

// Escaneia o código informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o código lido
int input_code();

// Escaneia o valor de uma string informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o valor lido lido
void input_string(char * label, char * value);


#endif //TRABALHO_3_PRODUCT_VIEW_H
