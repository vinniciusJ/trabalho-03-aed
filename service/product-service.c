//
// Created by gedt on 06/04/2024.
//
#include <string.h>
#include "product-service.h"
#include "../utils/headers/b-tree.h"
#include "../views/headers/product-view.h"
#include "../utils/headers/queue.h"

int is_leaf(ProductNode * tree){
    return tree->children[0] == -1;
}

// Realiza o split de nós cheios
// Pré-condição: posição do nó e arquivo de índíces aberto para leitura e escrita
// Pós-condição: retorna a nova posição, a chave promovida e a posição da chave promovida
int split(int position, int *key, int * data_position, FILE *file) {
    IndexHeader *header = read_header(sizeof(IndexHeader), file);

    ProductNode * x = (ProductNode *) read_node(position, sizeof(ProductNode), sizeof(IndexHeader), file);
    ProductNode * y = (ProductNode *) alloc(sizeof(ProductNode));

    int middle = x->keys_length / 2;

    y->keys_length = x->keys_length - middle - 1;
    x->keys_length = middle;

    *key = x->keys[middle];
    *data_position = x->data[middle];

    y->children[0] = x->children[middle + 1];

    for (int i = 0; i < y->keys_length; i++) {
        y->keys[i] = x->keys[middle + 1 + i];
        y->data[i] = x->data[middle + 1 + i];

        y->children[i + 1] = x->children[middle + 2 + i];
    }

    int y_position = header->top + 1;
    header->top++;

    set_node(x, sizeof(ProductNode), sizeof(IndexHeader), position, file);
    set_node(y, sizeof(ProductNode), sizeof(IndexHeader), y_position, file);

    set_header(header, sizeof(IndexHeader), file);

    free(x);
    free(header);

    return y_position;
}

// Insere um produto no arquivo de dados e retorna a posição no arquivo
// Pré-condição: um produto válido
// Pós-condição: produto inserido no arquivo de dados
int insert_at_data_file(Product * product, FILE * data_file){
    DataHeader * data_header = read_header(sizeof(DataHeader), data_file);

    int node_position = data_header->top;

    set_node(product, sizeof(Product), sizeof(DataHeader), data_header->top++, data_file);
    set_header(data_header, sizeof(DataHeader), data_file);

    return node_position;
}

// Verifica se a chave está no nó e retorna a posição dela
// Pré-condição: uma posição inicial e o arquivo de indices aberto para leitura e escrita
// Pós-condição: retorna se a chave está
int search_position(int key, int *position, int start_position, FILE *file) {
    ProductNode *node = (ProductNode *)read_node(start_position, sizeof(ProductNode), sizeof(IndexHeader), file);

    if (node == NULL) {
        return 0;
    }

    for ((*position) = 0; *position < node->keys_length; (*position)++) {
        int current_key = node->keys[*position];

        if (key == current_key) {
            return 1;
        } else if (key < current_key) {
            break;
        }
    }

    return 0;
}

// Adiciona uma chave em um nó não cheio
// Pré-condição: posição do nó, chave, posição da chave no arquivo de dados e um arquivo de índices aberto para leitura e escrita
// Pós-condição: a chave é inserida no nó
void add_node_at_right(int key_position, int key, int data_position, int child_position, int node_position, FILE *file) {
    ProductNode *node = (ProductNode *)read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), file);

    if (node == NULL) return;

    for (int i = node->keys_length; i > key_position; i--) {
        node->keys[i] = node->keys[i - 1];
        node->data[i] = node->data[i - 1];
        node->children[i + 1] = node->children[i];
    }

    node->keys[key_position] = key;
    node->data[key_position] = data_position;
    node->children[key_position + 1] = child_position;

    node->keys_length++;

    set_node(node, sizeof(ProductNode), sizeof(IndexHeader), node_position, file);

    free(node);
}

// Verica se o nó está em overflow
// Pré-condição: um nó não-nulo
// Pós-condição: retorna se o nó está ou não em overflow
int is_overflow(ProductNode * node){
    return node->keys_length == ORDER;
}

// Função auxiliar para inserção
// Pré-condição: posição do dados noa arquivo de dados,a posição do nó pai e um arquivo aberto para leitura e escrita
// Pós-condição: nó inserido nos arquivos
void insert_node(int key, int data_position, int node_position, FILE *file) {
    int position = 0;

    ProductNode *node = (ProductNode *)read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), file);

    if (node == NULL) return;

    int found = search_position(key, &position, node_position, file);

    if(is_leaf(node)){
        if(!found){
            add_node_at_right(position, key, data_position, -1, node_position, file);
        }
    }
    else {
        if(!found){
            insert_node(key, data_position, node->children[position], file);

            ProductNode *child_node = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), file);

            if (is_overflow(child_node)) {

                int middle_key = 0;
                int middle_data_position = 0;

                int new_node_position = split(node->children[position], &middle_key, &middle_data_position, file);

                add_node_at_right(position, middle_key, middle_data_position, new_node_position, node_position, file);
            }

            free(child_node);
        }
    }

    free(node); // Clean up
}

// Insere um produto no arquivo de indices e no de dados
// Pós-condição: produto válido e arquivos de dados e indices abertos pra escritura e escrita
// Pré-condição: produto inserido com sucesso nos dois arquivos
void insert(int key, Product * product, FILE * data_file, FILE* index_file) {
    IndexHeader* header = read_header(sizeof(IndexHeader), index_file);

    int data_position = insert_at_data_file(product, data_file);

    if (header->root == -1) {
        ProductNode* node = (ProductNode*) alloc(sizeof(ProductNode));

        node->keys[0] = key;
        node->data[0] = data_position;

        node->keys_length = 1;

        for(int i = 0; i <= ORDER; i++){
            node->children[i] = -1;
        }

        header->root = header->top++;

        set_header(header, sizeof(IndexHeader), index_file);
        set_node(node, sizeof(ProductNode), sizeof(IndexHeader), header->root, index_file);

        free(node);
    } else {
        insert_node(key, data_position, header->root, index_file);

        ProductNode* root = (ProductNode*) read_node(header->root, sizeof(ProductNode), sizeof(IndexHeader), index_file);

        if (is_overflow(root)) {

            int middle_key = 0;
            int middle_data_position = 0;

            int new_node_position = split(header->root, &middle_key, &middle_data_position, index_file);

            IndexHeader * new_header = read_header(sizeof(IndexHeader), index_file);
            ProductNode* new_root = (ProductNode*) alloc(sizeof(ProductNode));

            new_root->keys[0] = middle_key;
            new_root->data[0] = middle_data_position;

            new_root->children[0] = new_header->root;
            new_root->children[1] = new_node_position;
            new_root->keys_length = 1;

            for(int i = 2; i <= ORDER; i++){
                new_root->children[i] = -1;
            }

            new_header->root = new_header->top + 1;
            new_header->top++;


            set_node(new_root, sizeof(ProductNode), sizeof(IndexHeader), new_header->root, index_file);

            set_header(new_header, sizeof(IndexHeader), index_file);

            free(new_header);
            free(new_root);
        }
        free(root);
    }
    free(header);
}

// Busca um nó de um produto pela chave
// Pré-condição: ponteiro para a posição da chave no nó e arquivo aberto p/ leitura e escrita
// Pós-condição: retorna o nó do produto ou NULL
ProductNode * search(int key, int * position, int root_position, FILE * index_file){
    if(root_position == -1){
        return NULL;
    }

    int index = 0;
    ProductNode * node = read_node(root_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    while(index < node->keys_length && node->keys[index] < key){
        index++;
    }

    if(index + 1 > node->keys_length || node->keys[index] > key){
        if(!is_leaf(node)){
            return search(key, position, node->children[index], index_file);
        }
    }

    *position = index;

    return node;
}

// Busca um produto pela chave
// Pré-condição: arquivos de indices e dados abertos para leitura e escrita
// Pós-condição: retorna o produto ou NULL
Product * get_product_by_code(int key, FILE * data_file, FILE * index_file){
    IndexHeader * index_header = read_header(sizeof(IndexHeader), index_file);

    int data_position = 0;

    ProductNode * node = search(key, &data_position, index_header->root, index_file);

    if(node == NULL){
        return NULL;
    }

    return read_node(node->data[data_position], sizeof(Product), sizeof(DataHeader), data_file);
}

// Edita o preço de um produto
// Pré-condição: a chave do produto, o novo preços e os arquivos de dados e indices abertos para leitura e escrita
// Pós-condição: o preço do produto é atualizado no arquivo de dados
void update_product_price(int key, double price, FILE * data_file, FILE * index_file){
    IndexHeader * index_header = read_header(sizeof(IndexHeader), index_file);

    int data_position = 0;

    ProductNode * node = search(key, &data_position, index_header->root, index_file);

    if(node == NULL){
        return;
    }

    Product * product = read_node(node->data[data_position], sizeof(Product), sizeof(DataHeader), data_file);

    product->price = price;

    set_node(product, sizeof(Product), sizeof(DataHeader), node->data[data_position], data_file);
}

// Edita o estoque de um produto
// Pré-condição: a chave do produto, o novo estoque e os arquivos de dados e indices abertos para leitura e escrita
// Pós-condição: o estoque do produto é atualizado no arquivo de dados
void update_product_quantity(int key, int quantity, FILE * data_file, FILE * index_file){
    IndexHeader * index_header = read_header(sizeof(IndexHeader), index_file);

    int data_position = 0;

    ProductNode * node = search(key, &data_position, index_header->root, index_file);

    if(node == NULL){
        return;
    }

    Product * product = read_node(node->data[data_position], sizeof(Product), sizeof(DataHeader), data_file);

    product->quantity = quantity;

    set_node(product, sizeof(Product), sizeof(DataHeader), node->data[data_position], data_file);
}

// Imprime a arvore de produtos ordenado crescentemente pelo código
// Pre-condição: arquivos de dados e indices aberto p/ leitura e escrita
// Pós-condição: todos os produtos são listados no terminal ordenados pelo código
void show_products(int position, FILE * data_file, FILE * index_file){
    if(position == -1){
        return;
    }

    ProductNode * node = read_node(position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    int i = 0;

    for(i = 0; i < node->keys_length; i++){
        if(!is_leaf(node)){
            show_products(node->children[i], data_file, index_file);
        }

        Product * product = read_node(node->data[i], sizeof(Product), sizeof(DataHeader), data_file);

        show_product(product);
        free_space(product);
    }

    if(!is_leaf(node)){
        show_products(node->children[i], data_file, index_file);
    }

    free_space(node);
}

// Imprime os códigos dos produtos formatados como uma arvore B
// Pre-condição: arquivos de dados e indices aberto p/ leitura e escrita
// Pós-condição: os códigos dos produtos são listados como uma arvore B no terminal
void show_products_code(int root_position, FILE * index_file){
    if(root_position == -1){
        return;
    }

    ProductNode * root = read_node(root_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    Queue * queue = create_queue();

    enqueue(root, queue);

    while(!is_queue_empty(queue)){
        int nodes_length = get_queue_length(queue);

        for(int i = 0; i < nodes_length; i++){
            ProductNode * current_node = dequeue(queue);

            char separator = '[';

            for(int j = 0; j < current_node->keys_length; j++){
                printf("%c%d", separator, current_node->keys[j]);
                separator = ',';
            }

            printf("]");

            for(int j = 0; j <= current_node->keys_length; j++){
                if(!is_leaf(current_node) && current_node->children[j] != -1){
                    ProductNode * child = read_node(current_node->children[j], sizeof(ProductNode), sizeof(IndexHeader), index_file);

                    enqueue(child, queue);
                }
            }

            free_space(current_node);
        }

        printf("\n");
    }

    free(queue);
}

// Atualiza o dado de um produto a partir de uma string de operação em lote
// Pré-condição: string da operação e arquivos de indices e dados abertos para leitura e escrita
// Pós-codição: produto é atualizado com sucesso
void update_by_string(const char * string, FILE * data_file, FILE * index_file){
    int result = 0, code = 0, quantity = 0;
    double price = 0;

    result = sscanf(string, "A;%d;%d;", &code, &quantity);

    if(result == 2){
        update_product_quantity(code, quantity, data_file, index_file);

        result = sscanf(string, "A;%d;%d;%lf", &code, &quantity, &price);

        if(result == 3){
            update_product_price(code, price, data_file, index_file);
        }
    }
    else {
        sscanf(string, "A;%d;;%lf", &code, &price);

        update_product_price(code, price, data_file, index_file);
    }
}

// Insere um produto a partir de uma string de operação em lote
// Pré-condição: string da operação e arquivos de indices e dados abertos para leitura e escrita
// Pós-codição: produto é inserido com sucesso
void insert_by_string(const char * string, FILE * data_file, FILE * index_file){
    Product * product = (Product *) alloc(sizeof(Product));

    //Troca , por . para que o preço seja reconhecido como double
    char *comma_ptr = strchr(string, ',');
    if (comma_ptr != NULL) {
        *comma_ptr = '.';
    }

    sscanf(string, "I;%d;%[^;];%[^;];%[^;];%d;%lf", &product->code, product->name, product->brand, product->category, &product->quantity, &product->price);

    insert(product->code, product, data_file, index_file);

    free_space(product);
}

// Executa as operações em lote
// Pré-condição: arquivo das operações, de dados e de indices abertos para escrita e leitura
// Pós-condição: todas as operações são executadas
void execute_batch_operations(FILE * data, FILE * data_file, FILE * index_file){
    char operation;
    char string[256];

    while(fgets(string, sizeof(string), data) != NULL){
        if(sscanf(string, " %c", &operation) != 1){
            return;
        }

        if(operation == 'I'){
           insert_by_string(string, data_file, index_file);
        }
//        if(operation == 'A'){
//            update_by_string(string, data_file, index_file);
//        }
    }
}

int find_key_position(int key, int node_position, FILE * index_file){
    int position = 0;

    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    while(position < node->keys_length && node->keys[position] < key){
        position++;
    }

    free_space(node);

    return position;
}

void remove_from_leaf(int position, int node_position, FILE * index_file){
    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    for(int i = position + 1; i < node->keys_length; i++){
        node->keys[i - 1] = node->keys[i];
    }

    node->keys_length--;

    set_node(node, sizeof(ProductNode), sizeof(IndexHeader), node_position, index_file);
}

int get_previous_bigger(int position, int node_position, FILE * index_file){
    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * current = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), index_file);

    while(!is_leaf(current)){
        free_space(current);
        current = read_node(node->children[current->keys_length], sizeof(ProductNode), sizeof(IndexHeader), index_file);
    }

    return current->keys[current->keys_length - 1];
}

int get_next_smaller(int position, int node_position, FILE * index_file){
    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * current = read_node(node->children[position + 1], sizeof(ProductNode), sizeof(IndexHeader), index_file);

    while(!is_leaf(current)){
        free_space(current);
        current = read_node(node->children[0], sizeof(ProductNode), sizeof(IndexHeader), index_file);
    }

    free_space(node);

    return current->keys[0];
}

void remove_from_non_leaf(int position, int node_position, FILE * index_file){
    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    int key = node->keys[position];

    ProductNode * child = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * next_child = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), index_file);

    if(child->keys_length >= MIN_KEYS){
        int previous_bigger = get_previous_bigger(position, node_position, index_file);

        node->keys[position] = previous_bigger;

        set_node(node, sizeof(ProductNode), sizeof(IndexHeader), node_position, index_file);

        remove_key(previous_bigger, node->children[position], index_file);
    }
    else if(next_child->keys_length >= MIN_KEYS){
        int next_smaller = get_next_smaller(position, node_position, index_file);

        node->keys[position] = next_smaller;

        set_node(node, sizeof(ProductNode), sizeof(IndexHeader), node_position, index_file);

        remove_key(next_smaller, node->children[position + 1], index_file);
    }
    else {
        merge_nodes(position, node_position, index_file);
        remove_key(key, node->children[position], index_file);
    }
}

void borrow_from_previous(int position, int node_position, FILE * index_file){
    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    ProductNode * child = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * sibling = read_node(node->children[position - 1], sizeof(ProductNode), sizeof(IndexHeader), index_file);

    for(int i = child->keys_length - 1; i >= 0; --i){
        child->keys[i + 1] = child->keys[i];
    }

    if(!is_leaf(child)){
        for(int i = child->keys_length; i >= 0; --i){
            child->children[i + 1] = child->children[i];
        }
    }

    child->keys[0] = sibling->keys[position - 1];

    if(!is_leaf(sibling)){
        child->children[0] = sibling->children[sibling->keys_length];
    }

    node->keys[position - 1] = sibling->keys[sibling->keys_length - 1];

    child->keys_length += 1;
    sibling->keys_length += 1;

    set_node(node, sizeof(ProductNode), sizeof(IndexHeader), node_position, index_file);
    set_node(child, sizeof(ProductNode), sizeof(IndexHeader), node->children[position], index_file);
    set_node(sibling, sizeof(ProductNode), sizeof(IndexHeader), node->children[position - 1], index_file);
}

void borrow_from_next(int position, int node_position, FILE * index_file) {
    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    ProductNode * child = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * sibling = read_node(node->children[position + 1], sizeof(ProductNode), sizeof(IndexHeader), index_file);

    child->keys[child->keys_length] = node->keys[position];

    if (!is_leaf(child)) {
        child->children[child->keys_length + 1] = sibling->children[0];
    }

    node->keys[position] = sibling->keys[0];

    for (int i = 1; i < sibling->keys_length; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    if (!is_leaf(sibling)) {
        for (int i = 1; i <= sibling->keys_length; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    sibling->keys_length -= 1;
    child->keys_length += 1;

    set_node(node, sizeof(ProductNode), sizeof(IndexHeader), node_position, index_file);
    set_node(child, sizeof(ProductNode), sizeof(IndexHeader), node->children[position], index_file);
    set_node(sibling, sizeof(ProductNode), sizeof(IndexHeader), node->children[position + 1], index_file);
}

void merge_nodes(int position, int node_position, FILE * index_file){
    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    ProductNode * child = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * sibling = read_node(node->children[position + 1], sizeof(ProductNode), sizeof(IndexHeader), index_file);

    child->keys[child->keys_length] = node->keys[position];

    for (int i = 0; i < sibling->keys_length; ++i) {
        child->keys[i + child->keys_length + 1] = sibling->keys[i];

        if (!is_leaf(child)) {
            child->children[i + child->keys_length + 1] = sibling->children[i];
        }
    }
    if (!is_leaf(sibling)) {
        child->children[child->keys_length + sibling->keys_length + 1] = sibling->children[sibling->keys_length];
    }

    // Updating the node key positions
    for (int i = position + 1; i < node->keys_length; ++i) {
        node->keys[i - 1] = node->keys[i];
        node->children[i] = node->children[i + 1];
    }

    node->keys_length--;
    child->keys_length += sibling->keys_length + 1;

    set_node(node, sizeof(ProductNode), sizeof(IndexHeader), node_position, index_file);
    set_node(child, sizeof(ProductNode), sizeof(IndexHeader), node->children[position], index_file);
    set_node(sibling, sizeof(ProductNode), sizeof(IndexHeader), node->children[position + 1], index_file);
}

//void fill(int position, int node_position, FILE * index_file){
//    ProductNode * node = read_node(node_position, sizeof(ProductNode), sizeof(IndexHeader), index_file);
//
//    if(position != 0 && node->children[position - 1]->keysLength >= MIN_KEYS){
//        borrow_from_previous(position, node_position, index_file);
//    }
//    else if(position != tree->keysLength && tree->children[position + 1]->keysLength >= MIN_KEYS){
//        borrow_from_next(position, tree);
//    }
//    else {
//        if(position != tree->keysLength){
//            merge_nodes(position, tree);
//        }
//        else{
//            merge_nodes(position - 1, tree);
//        }
//    }
//}
void fill(int position, int node_pos, FILE *file) {
    ProductNode *node = read_node(node_pos, sizeof(ProductNode), sizeof(IndexHeader), file);

    if (position != 0 && node->children[position - 1] != -1) {
        ProductNode *leftSibling = read_node(node->children[position - 1], sizeof(ProductNode), sizeof(IndexHeader), file);

        if (leftSibling->keys_length >= MIN_KEYS) {
            borrow_from_previous(position, node_pos, file);
        }

        free(leftSibling);
    } else if (position != node->keys_length && node->children[position + 1] != -1) {
        ProductNode *rightSibling = read_node(node->children[position + 1], sizeof(ProductNode), sizeof(IndexHeader), file);

        if (rightSibling->keys_length >= MIN_KEYS) {
            borrow_from_next(position, node_pos, file);
        }

        free(rightSibling);
    } else {
        if (position != node->keys_length) {
            merge_nodes(position, node_pos, file);
        } else {
            merge_nodes(position - 1, node_pos, file);
        }
    }
    free(node);
}

void remove_key(int key, int node_pos, FILE *file) {
    ProductNode *node = read_node(node_pos, sizeof(ProductNode), sizeof(IndexHeader), file);

    int position = find_key_position(key, node_pos, file);

    if (position < node->keys_length && node->keys[position] == key) {
        if (is_leaf(node)) {
            remove_from_leaf(position, node_pos, file);
        } else {
            remove_from_non_leaf(position, node_pos, file);  // Esta função também precisará ser adaptada
        }
    } else {
        if (is_leaf(node)) {
            free(node);
            return;
        }

        int is_full = position == node->keys_length;

        ProductNode *found_node = read_node(node->children[position], sizeof(ProductNode), sizeof(IndexHeader), file);

        if (found_node->keys_length < MIN_KEYS) {
            fill(position, node_pos, file);  // Esta função também precisará ser adaptada
        }

        if (is_full && position > node->keys_length) {
            remove_key(key, node->children[position - 1], file);
        } else {
            remove_key(key, node->children[position], file);
        }
    }

    free(node);
}
