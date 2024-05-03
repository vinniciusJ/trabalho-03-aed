//
// Created by gedt on 06/04/2024.
//
#include <string.h>
#include "product-service.h"
#include "../utils/headers/b-tree.h"
#include "../views/headers/product-view.h"
#include "../utils/headers/queue.h"

// Verifica se um no é folha
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

// Verifica se a chave está no nó e atualiza a posição dela
// Pré-condição: uma posição inicial e o arquivo de indices aberto para leitura e escrita
// Pós-condição: retorna se a chave está no nó e atualiza position
int search_position_in_node(int key, int *position, int node_pos, FILE *file) {
    ProductNode *node = (ProductNode *)read_node(node_pos, sizeof(ProductNode), sizeof(IndexHeader), file);

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

// Busca um a posição de um nó no aquivo de indices
int search_node(int key, FILE * file) {
    IndexHeader *header = read_header(sizeof(IndexHeader), file);
    int root = header->root;
    free(header);

    if(root == -1) return -1;

    return search_node_aux(key, root, file);
}

// Função recursiva auxiliar para buscar a posição de um nó no arquivo de indicies
int search_node_aux(int key, int position, FILE * file) {
    if(position == -1) return -1;

    ProductNode * node = read_node(position, sizeof(ProductNode), sizeof(IndexHeader), file);

    int i, curr_position;

    for(i = 0; i < node->keys_length; i++) {
        if(node->keys[i] == key) {
            free(node);
            return position;
        } else if( node->keys[i] > key) {
            curr_position = node->children[i];
            free(node);
            return search_node_aux(key, curr_position, file);
        }
    }

    curr_position = node->children[i];
    free(node);

    return search_node_aux(key, curr_position, file);
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

    int found = search_position_in_node(key, &position, node_position, file);

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
        return search(key, position, node->children[index], index_file);
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

// Função para remover um produto
void remove_product(int key, FILE * index_file, FILE * data_file) {
    IndexHeader *header = read_header(sizeof(IndexHeader), index_file);

    if(header->root == -1) {
        free(header);
        return;
    };

    int position = search_node(key, index_file);
    if(position == -1) {
        free(header);
        return;
    }

    remove_key(key, header->root, position, index_file, data_file);

}

void update_removed_leaf_node(ProductNode * leaf, int remove_pos, int remove_pos_in_node, FILE * index_file, FILE * data_file){
    IndexHeader *data_header = read_header(sizeof(DataHeader), data_file);

    int data_pos = leaf->data[remove_pos_in_node];

    // atualizar posições livres TODO

    int i;
    for(i = remove_pos_in_node; i < leaf->keys_length - 1; i++) {
        leaf->keys[i] = leaf->keys[i+1];
        leaf->data[i] = leaf->data[i+1];
        leaf->children[i] = leaf->children[i+1];
    }

    leaf->children[i] = leaf->children[leaf->keys_length];
    leaf->keys_length--;

    set_node(leaf, sizeof(ProductNode), sizeof(IndexHeader), remove_pos, index_file);
    free(data_header);
}

// Remove para o 1° caso: a remoção é feita em um nó folha com número de chaves maior que o mínimo
void remove_case1(ProductNode * remove_node, int key, int remove_pos, FILE * index_file, FILE * data_file) {
    int key_pos = 0;

    search_position_in_node(key, &key_pos, remove_pos, index_file);
    update_removed_leaf_node(remove_node, remove_pos, key_pos, index_file, data_file);
}

// Verifica se o no tem mais chaves do que o mínimo
int has_more_than_min_keys(ProductNode * node) {
    return node->keys_length > MIN_KEYS;
}

// Busca a chave sucessora em um nó folha
int search_next_key_leaf(ProductNode * remove_node, int key_pos, int * next_node_pos, FILE *index_file) {
    int next_key;
    *next_node_pos = remove_node->children[key_pos + 1];

    ProductNode * next_node =  read_node(*next_node_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    if(is_leaf(next_node)) {
        next_key = next_node->keys[0];
        free(next_node);
        return next_key;
    }

    return search_next_key_leaf(next_node, -1, next_node_pos, index_file);
}

// Busca o pai de um nó na árvore B a partir de um código de produto
int search_father(int key, FILE * index_file) {
    IndexHeader *header = read_header(sizeof(IndexHeader), index_file);

    int root_pos = header->root;
    free(header);

    int key_pos, found;
    found = search_position_in_node(key,  &key_pos, root_pos, index_file);

    ProductNode * root = read_node(root_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    if(is_leaf(root) || found) {
        free(root);
        return -1;
    }

    free(root);
    return 0; // função auxiliar TODO
}

int search_father_aux(int key, int root_pos, FILE * index_file){
    ProductNode * root = read_node(root_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    int i;
    for(i = 0; i < root->keys_length; i++) {
        if(key < root->keys[i]) {
            int key_pos, found;
            found = search_position_in_node(key,  &key_pos, root->children[i], index_file);

            if(found) return key_pos;

            return search_father_aux(key, root->children[i], index_file);
        }
    }

    int key_pos, found;
    found = search_position_in_node(key, &key_pos, root->children[i], index_file);

    if(found) return root_pos;

    return search_father_aux(key, root->children[i], index_file);
}

void remove_key(int key, int root_pos, int remove_pos, FILE *index_file, FILE * data_file) {
    IndexHeader *index_header = read_header(sizeof(IndexHeader), index_file);
    IndexHeader *data_header = read_header(sizeof(DataHeader), data_file);

    ProductNode *remove_node = read_node(remove_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    if(is_root(remove_pos, index_file) && is_leaf(remove_node)){
        remove_case1(remove_node, key, remove_pos, index_file, data_file);
        if(remove_node->keys_length == 0) {
            index_header->root = -1;
            index_header->top = 0;
            index_header->free = -1;
            set_header(index_header, sizeof(IndexHeader), index_file);

            data_header->top = 0;
            data_header->free = -1;
            set_header(data_header, sizeof(IndexHeader), data_file);
        }
    }

    //Refatorar TODO
    else if(has_more_than_min_keys(remove_node) && is_leaf(remove_node)) {
        remove_case1(remove_node, key, remove_pos, index_file, data_file);
    }

    else if(!is_leaf(remove_node)) {
        int key_pos, next_pos_aux, next_key, next_pos, father_pos;
        search_position_in_node(key, &key_pos, remove_pos, index_file);
        next_key = search_next_key_leaf(remove_node, key_pos, &next_pos_aux, index_file);
        father_pos = search_father(key, index_file);

        //next_pos TODO
    }

    free(index_header);
    free(data_header);
}