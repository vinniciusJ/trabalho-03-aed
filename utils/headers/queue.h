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

int is_queue_empty(Queue * queue);
QueueNode * create_queue_node(ProductNode * value);
Queue * create_queue();
void enqueue(ProductNode * value, Queue * queue);
ProductNode * dequeue(Queue * queue);
int get_queue_length(Queue * queue);


#endif //TRABALHO_3_QUEUE_H
