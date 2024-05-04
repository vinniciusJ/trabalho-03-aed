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


// Atualiza a lista de posições livres no arquivo de dados após a remoção de um registro
void update_free_positions_data(int position, FILE * data_file) {
    DataHeader *data_header = read_header(sizeof(DataHeader), data_file);
    Product * product = read_node(position, sizeof(Product), sizeof(DataHeader), data_file);

    if (product != NULL) {
        product->code = data_header->free;  //a posição do proximo livre é guardada no campo código
        set_node(product, sizeof(Product), sizeof(DataHeader), position, data_file);
        data_header->free = position;
        set_header(data_header, sizeof(DataHeader), data_file);
        free(product);
    }
    free(data_header);
}

// Atualiza a lista de posições livres no arquivo de índices após a remoção de um nó
void update_free_positions_index(int position, FILE * index_file) {
    IndexHeader *index_header = read_header(sizeof(IndexHeader), index_file);
    ProductNode *node = read_node(position, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    if(node != NULL) {
        node->keys[0] = index_header->free; //a posição do proximo livre é guardada na primeira chave
        set_node(node, sizeof(ProductNode), sizeof(IndexHeader), position, index_file);
        index_header->free = position;
        set_header(index_header, sizeof(IndexHeader), index_file);
        free(node);
    }
    free(index_header);
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

    update_free_positions_data(data_pos, data_file);

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

// Remove o 1° caso: a remoção é feita em um nó folha com número de chaves maior que o mínimo
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

// Função auxiliar para buscar o pai de um nó na árvore B a partir de um código de produto
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
    return search_father_aux(key, root_pos, index_file);
}

// Busca a posição de um filho em um nó
int search_son_pos(ProductNode * node, int pos) {
    int son;

    for(son = 0; son <= node->keys_length; son++) {
        if(node->children[son] == pos) return son;
    }

    return -1;
}

// Remove caso 2: nó interno
int remove_case2(int key, int remove_pos, ProductNode * remove_node, FILE *index_file, FILE * data_file) {
    int key_pos, next_pos, next_key;
    search_position_in_node(key,  &key_pos, remove_pos, index_file);

    next_key = search_next_key_leaf(remove_node, key_pos, &next_pos, index_file);

    ProductNode *next = read_node(next_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    int remove_data = next->data[0];
    update_removed_leaf_node(next, next_pos, 0, index_file, data_file);

    remove_node->keys[key_pos] = next_key;
    remove_node->data[key_pos] = remove_data;
    set_node(remove_node, sizeof(ProductNode), sizeof(IndexHeader), remove_pos, index_file);

    free(next);
    return next_pos;
}

// Verifica se é possível realizar a redistribuição de chaves entre irmãos
int can_redistribute(int father_pos, int son_index, int * left_pos, int * right_pos, FILE *index_file) {
    int result = 0;
    ProductNode * left, * right, * father = read_node(father_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    if(son_index == 0) {
        right = read_node(father->children[1], sizeof(ProductNode), sizeof(IndexHeader), index_file);

        if(right->keys_length > MIN_KEYS) {
            *left_pos = -1;
            *right_pos = father->children[1];
            free(right);
            result = 1;
        }
    }
    else if(son_index == father->keys_length) {
        left = read_node(father->children[father->keys_length - 1], sizeof(ProductNode), sizeof(IndexHeader), index_file);

        if(left->keys_length > MIN_KEYS) {
            *left_pos = father->children[father->keys_length - 1];
            *right_pos = -1;
            free(left);
            result = 1;
        }
    }
    else {
        left = read_node(father->children[son_index - 1], sizeof(ProductNode), sizeof(IndexHeader), index_file);
        right = read_node(father->children[son_index + 1], sizeof(ProductNode), sizeof(IndexHeader), index_file);

        if(right->keys_length > MIN_KEYS) {
            *left_pos = -1;
            *right_pos = father->children[son_index + 1];
            result = 1;
        } else if (left->keys_length > MIN_KEYS) {
            *left_pos = father->children[son_index - 1];
            *right_pos = -1;
            result = 1;
        }
        free(left);
        free(right);
    }

    if(result != 1) {
        *left_pos = -1;
        *right_pos = -1;
    }

    free(father);
    return result;
}

// Redistribui chaves a partir do irmão direito
void redistribute_right(int father_pos, int remove_pos, int remove_son_pos, int right_pos,  FILE *index_file) {
    ProductNode * father = read_node(father_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * remove_node = read_node(remove_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * right = read_node(right_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    remove_node->keys[remove_node->keys_length] = father->keys[remove_son_pos];
    remove_node->data[remove_node->keys_length] = father->data[remove_son_pos];
    remove_node->keys_length++;
    remove_node->children[remove_node->keys_length] = right->children[0];

    father->keys[remove_son_pos] = right->keys[0];
    father->data[remove_son_pos] = right->data[0];

    int i;
    for(i = 0; i < right->keys_length - 1; i++) {
        right->keys[i] = right->keys[i + 1];
        right->data[i] = right->data[i + 1];
        right->children[i] = right->children[i + 1];
    }
    right->children[i] = right->children[right->keys_length];
    right->keys_length++;

    set_node(father, sizeof(ProductNode), sizeof(IndexHeader), father_pos, index_file);
    set_node(remove_node, sizeof(ProductNode), sizeof(IndexHeader), remove_pos, index_file);
    set_node(right, sizeof(ProductNode), sizeof(IndexHeader), right_pos, index_file);

    free(father);
    free(remove_node);
    free(right);
}

// Redistribui chaves a partir do irmão esquerdo
void redistribute_left(int father_pos, int remove_pos, int remove_son_pos, int left_pos, FILE *index_file) {
    ProductNode * father = read_node(father_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * remove_node = read_node(remove_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * left = read_node(left_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    int i;
    for(i = remove_node->keys_length; i > 0; i--) {
        remove_node->keys[i] = remove_node->keys[i - 1];
        remove_node->data[i] = remove_node->data[i - 1];
        remove_node->children[i] = remove_node->children[i];
    }

    remove_node->children[1] = remove_node->children[0];
    remove_node->keys[0] = father->keys[remove_son_pos - 1];
    remove_node->data[0] = father->data[remove_son_pos - 1];
    remove_node->keys_length++;

    father->keys[remove_son_pos - 1] = left->keys[left->keys_length - 1];
    father->data[remove_son_pos - 1] = left->data[left->keys_length - 1];

    remove_node->children[0] = left->children[left->keys_length];
    left->keys_length--;

    set_node(father, sizeof(ProductNode), sizeof(IndexHeader), father_pos, index_file);
    set_node(remove_node, sizeof(ProductNode), sizeof(IndexHeader), remove_pos, index_file);
    set_node(left, sizeof(ProductNode), sizeof(IndexHeader), left_pos, index_file);

    free(father);
    free(remove_node);
    free(left);
}

// Realiza a redistribuição de chaves entre irmãos
void redistribute(int father_pos, int remove_pos, int remove_son_pos, int left_pos, int right_pos,  FILE *index_file) {
    if(right_pos != -1) {
        redistribute_right(father_pos, remove_pos, remove_son_pos, right_pos, index_file);
    } else {
        redistribute_left(father_pos, remove_pos, remove_son_pos, left_pos, index_file);
    }
}

// Busca os filhos esquerdo e direito de um nó pai
void search_leaft_right_children(int father_pos, int remove_pos, int * left_pos, int * right_pos,  FILE *index_file) {
    ProductNode * father = read_node(father_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

    if(remove_pos == 0) {
        *right_pos = father->children[remove_pos + 1];
        *left_pos = father->children[remove_pos];
    }
    else if (remove_pos == father->keys_length) {
        *left_pos = father->children[father->keys_length - 1];
        *right_pos = -1;
    }
    else {
        *left_pos = father->children[remove_pos - 1];
        *right_pos = father->children[remove_pos + 1];
    }

    free(father);
}

// Concatena o nó da esquerda com o nó a ser removido
void combine_left(ProductNode * father, ProductNode * left, ProductNode * remove_node, int son_index, FILE * index_file){
    int i;

    left->keys[left->keys_length] = father->keys[son_index - 1];
    left->data[left->keys_length] = father->data[son_index - 1];
    left->keys_length++;

    for(i = 0; i < remove_node->keys_length; i++){
        left->keys[left->keys_length] = remove_node->keys[i];
        left->data[left->keys_length] = remove_node->data[i];
        left->children[left->keys_length] = remove_node->children[i];
        left->keys_length++;
    }

    left->children[left->keys_length] = remove_node->children[i];

    update_free_positions_index(father->children[son_index], index_file);

    for(i = son_index; i < father->keys_length - 1; i++){
        father->keys[i] = father->keys[i + 1];
        father->data[i] = father->data[i + 1];
        father->children[i + 1] = father->children[i + 2];
    }

    father->keys_length--;
}

// Concatena o nó da direita com o nó a ser removido
void combine_right(ProductNode * father, ProductNode * right, ProductNode * remove_node, int son_index, FILE * index_file){
    int i;

    remove_node->keys[remove_node->keys_length] = father->keys[son_index];
    remove_node->data[remove_node->keys_length] = father->data[son_index];
    remove_node->keys_length++;

    for(i = 0; i < right->keys_length; i++){
        remove_node->keys[remove_node->keys_length] = right->keys[i];
        remove_node->data[remove_node->keys_length] = right->data[i];
        remove_node->children[remove_node->keys_length] = right->children[i];
        remove_node->keys_length++;
    }

    remove_node->children[remove_node->keys_length] = right->children[i];

    update_free_positions_index(father->children[son_index] + 1, index_file);

    for(i = son_index; i < father->keys_length - 1; i++){
        father->keys[i] = father->keys[i + 1];
        father->data[i] = father->data[i + 1];
        father->children[i + 1] = father->children[i + 2];
    }

    father->keys_length--;
}

// Concatena nós
void combine(int father_pos, int remove_pos, int son_index, int left_pos, int right_pos, FILE * index_file){
    ProductNode * father = read_node(father_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * remove_node = read_node(remove_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    ProductNode * left, * right;

    if(right_pos == -1){
        left = read_node(left_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

        combine_left(father, left, remove_node, son_index, index_file);
        set_node(father, sizeof(ProductNode), sizeof(IndexHeader), father_pos, index_file);
        set_node(left, sizeof(ProductNode), sizeof(IndexHeader), left_pos, index_file);
        free(left);
    }

    else {
        right = read_node(right_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

        combine_left(father, right, remove_node, son_index, index_file);
        set_node(father, sizeof(ProductNode), sizeof(IndexHeader), father_pos, index_file);
        set_node(remove_node, sizeof(ProductNode), sizeof(IndexHeader), remove_pos, index_file);
        free(right);
    }

    free(remove_node);
}

// Balancea a árvore após uma remoção
void balance(int father_pos, int son_index, int remove_pos,  FILE *index_file) {
    if(father_pos != -1) {
        int left_pos, right_pos;
        int can_redistribute_result = can_redistribute(father_pos, son_index, &left_pos, &right_pos, index_file);

        if(can_redistribute_result) {
            redistribute(father_pos, remove_pos, son_index, left_pos, right_pos, index_file);
        } else {
            search_leaft_right_children(father_pos, son_index, &left_pos, &right_pos, index_file);
            combine(father_pos, remove_pos, son_index, left_pos, right_pos, index_file);
        }
    }
}

// Verifica e atualiza um pai após uma remoção.
void stabilize_father(int position, FILE *index_file) {
    ProductNode * father = read_node(position, sizeof(ProductNode), sizeof(IndexHeader), index_file);
    int grandfather_pos, son_index;

    while(father->keys_length < MIN_KEYS && !is_root(position, index_file)) {
        grandfather_pos = search_father(position, index_file);
        ProductNode * grandfather = read_node(grandfather_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
        son_index = search_son_pos(grandfather, position);

        free(grandfather);

        balance(grandfather_pos, son_index, position, index_file);

        free(father);
        father = read_node(grandfather_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
        position = grandfather_pos;
    }

    if(father->keys_length == 0) {
        IndexHeader *index_header = read_header(sizeof(IndexHeader), index_file);
        index_header->root = father->children[0];
        set_header(index_header, sizeof(IndexHeader), index_file);
        free(index_header);
    }

    free(father);
}

void remove_key(int key, int root_pos, int remove_pos, FILE *index_file, FILE * data_file) {
    IndexHeader *index_header = read_header(sizeof(IndexHeader), index_file);
    DataHeader *data_header = read_header(sizeof(DataHeader), data_file);

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

        next_pos = remove_case2(key, remove_pos, remove_node, index_file, data_file);
        ProductNode *next = read_node(next_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);

        if(next->keys_length < MIN_KEYS) {
            ProductNode * nexts_father = read_node(father_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
            int son_index = search_son_pos(nexts_father, next_pos);
            free(nexts_father);

            balance(father_pos, son_index, next_pos, index_file);
            stabilize_father(father_pos, index_file);
        }
    }

    else if(!has_more_than_min_keys(remove_node) && is_leaf(remove_node)) {
        int father_pos = search_father(key, index_file);
        ProductNode * father = read_node(father_pos, sizeof(ProductNode), sizeof(IndexHeader), index_file);
        int son_index = search_son_pos(father, remove_pos);
        free(father);

        remove_case1(remove_node, key, remove_pos, index_file, data_file);

        if(remove_node->keys_length < MIN_KEYS) {
            balance(father_pos, son_index, remove_pos, index_file);
            stabilize_father(father_pos, index_file);
        }
    }

    free(index_header);
    free(data_header);
}

void show_free_positions_from_index_file(FILE *index_file) {
    IndexHeader *index_header = read_header(sizeof(IndexHeader), index_file);

    int i = index_header->free;
    while (i != -1) {
        show_free_position(i);
        ProductNode * node = read_node(i, sizeof(ProductNode), sizeof(IndexHeader), index_file);
        i = node->keys[0];
        free(node);
    }

    free(index_header);
}

void show_free_positions_from_data_file(FILE *data_file) {
    DataHeader *data_header = read_header(sizeof(DataHeader), data_file);

    int i = data_header->free;
    while (i != -1) {
        show_free_position(i);
        Product * node = read_node(i, sizeof(Product), sizeof(DataHeader), data_file);
        i = node->code;
        free(node);
    }

    free(data_header);
}