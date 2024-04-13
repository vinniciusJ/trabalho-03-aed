//
// Created by gedt on 12/04/2024.
//

#ifndef TRABALHO_3_QUEUE_H
#define TRABALHO_3_QUEUE_H

#include <stdio.h>

#include "../../models/product.h"

typedef struct QueueNode {
    ProductNode * value;
    struct QueueNode * next;
} QueueNode;

typedef struct Queue {
    struct QueueNode * front;
    struct QueueNode * rear;
} Queue;

// Verifica se a fila está vazia
// Pré-condição: uma fila
// Pós-condição: 1 se a fila estiver vazia e 0 se não estiver
int is_queue_empty(Queue * queue);

// Cria um nó pra fila
// Pré-condição: um nó de produto
// Pós-condição: nó criado
QueueNode * create_queue_node(ProductNode * value);

// Cria uma fila vazia
// Pré-condição: nenhuma
// Pós-condição: retorna um fila vazia
Queue * create_queue();

// Adiciona o nó a fila
// Pré-condição: nó valido e um fila
// Pós-condição: nó inserido na fila
void enqueue(ProductNode * value, Queue * queue);

// Retira um nó da fila
// Pré-condição: nenhuma
// Pós-condição: retorna o nó retirado
ProductNode * dequeue(Queue * queue);

// Calcula o tamanho da fila
// Pré-condição: nenhuma
// Pós-condição: retorna o tamanho da fila
int get_queue_length(Queue * queue);


#endif //TRABALHO_3_QUEUE_H
