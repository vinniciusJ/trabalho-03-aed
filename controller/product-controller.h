//
// Created by gedt on 09/04/2024.
//

#ifndef TRABALHO_3_PRODUCT_CONTROLLER_H
#define TRABALHO_3_PRODUCT_CONTROLLER_H

// Rota para fazer a inserção de um produto
// Pré-condição: nenhuma
// Pós-condição: produto inserido no arquivo de índices e de dados
void insert_route();

// Rota para mostrar todos os produtos ordenados pelo código
// Pré-condição: nenhuma
// Pós-condição: todos os produtos são mostrados no terminal
void show_products_route();

// Rota para mostrar os dados de um produto pelo código
// Pré-condição: nenhuma
// Pós-condição: produto é mostrado no terminal
void show_product_by_code_route();

// Rota para atualizar o preço de um produto
// Pré-condição: nenhuma
// Pós-condição: preço do produto atualizado
void update_price_route();

// Rota para atualizar o estoque de um produto
// Pré-condição: nenhuma
// Pós-condição: estoque do produto atualizado
void update_quantity_route();

// Rota para a mostrar a árvore B formada pelos códigos
// Pré-condição: nenhuma
// Pós-condição: árvore B dos códigos é mostrada no terminal
void show_products_codes_route();

// Rota para executar as operações em lote
// Pré-condição: nenhuma
// Pós-condição: operações em lote executadas
void execute_batch_operations_route();

void remove_product_route();

void show_free_positions_from_index_file_route();

void show_free_positions_from_data_file_route();

#endif //TRABALHO_3_PRODUCT_CONTROLLER_H
