 //
// Created by gedt on 12/04/2024.
//
#include <stdio.h>

#include "headers/queue.h"
#include "headers/utils.h"
#include "headers/b-tree.h"
#include "../models/product.h"

int is_queue_empty(Queue * queue){
    return queue->front == NULL;
}

QueueNode * create_queue_node(ProductNode * value){
    QueueNode * node = (QueueNode *) alloc(sizeof(QueueNode));

    node->value = value;
    node->next = NULL;

    return node;
}

Queue * create_queue(){
    Queue * queue = (Queue *) alloc(sizeof(Queue));

    queue->front = NULL;
    queue->rear = NULL;

    return queue;
}

void enqueue(ProductNode * value, Queue * queue){
    QueueNode * node = create_queue_node(value);

    if(queue->rear == NULL){
        queue->front = queue->rear = node;

        return;
    }

    queue->rear->next = node;
    queue->rear = node;
}

ProductNode * dequeue(Queue * queue){
    if(queue->front == NULL){
        return NULL;
    }

    QueueNode * temp = queue->front;
    queue->front = queue->front->next;

    if(queue->front == NULL){
        queue->rear = NULL;
    }

    ProductNode * value = (ProductNode *) malloc(sizeof(ProductNode));

    value = temp->value;

    free(temp);

    return value;
}

int get_queue_length(Queue * queue) {
    int size = 0;
    QueueNode * current = queue->front;

    while (current != NULL) {
        size++;
        current = current->next;
    }

    return size;
}


