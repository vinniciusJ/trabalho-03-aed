//
// Created by gedt on 11/04/2024.
//
#include <stdio.h>

#include "headers/product-view.h"

// Escaneia os valores de um produto pelo terminal informados pelo usuario
// Pré-condição: nenhuma
// Pós-condição: retorna um produto informado pelo usuário
Product * input_product(){
    Product * product = (Product *) alloc(sizeof(Product));

    printf("Codigo do produto: ");
    scanf("%d%*c", &product->code);

    printf("Nome do produto: ");
    scanf("%[^\n]%*c", product->name);

    printf("Marca do produto: ");
    scanf("%[^\n]%*c", product->brand);

    printf("Categoria do produto: ");
    scanf("%[^\n]%*c", product->category);

    printf("Quantidade em estoque: ");
    scanf("%d", &product->quantity);

    printf("Valor do produto (R$): ");
    scanf("%lf", &product->price);

    return product;
}

// Escaneia o valor de um preço informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o valor lido e a chave informado pelo usuario
double input_new_price(int * key){
    double price = 0.0;

    printf("Digite o codigo do produto: ");
    scanf("%d", key);

    printf("Novo preco (R$): ");
    scanf("%lf", &price);

    return price;
}

// Escaneia o código informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o código lido
int input_code(){
    int code = 0;

    printf("Digite o codigo do produto: ");
    scanf("%d", &code);

    return code;
}

// Escaneia o valor de um estoque informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o valor lido e a chave informado pelo usuario
int input_new_quantity(int * key){
    int quantity = 0;

    printf("Digite o codigo do produto: ");
    scanf("%d", key);

    printf("Digite o estoque do produto: ");
    scanf("%d", &quantity);

    return quantity;
}

// Escaneia o valor de uma string informado pelo usuário
// Pré-condição: nenhuma
// Pós-condição: retorna o valor lido lido
void input_string(char * label, char * value){
    printf("%s", label);
    scanf("%[^\n]%*c", value);
}

// Mostra o cabeçalho da tabela de produtos
// Pré-condição: nenhuma
// Pós-condição: mostra o cabeçalho da tabela de produtos no terminal
void show_products_header(){
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("|%-6s | %-50s | %-25s | %-25s | %-8s | %-10s|\n", "Codigo", "Nome", "Marca", "Categoria", "Estoque", "Valor");
    printf("|-------|----------------------------------------------------|---------------------------|---------------------------|----------|-----------|\n");
}

// Mostra um produto formado em uma tabela
// Pré-condição: nenhuma
// Pós-condição: mostra no terminal os dados de um produto
void show_product(Product * product){
    printf("|%-6d | %-50s | %-25s | %-25s | %-8d | %-10.2lf|\n", product->code, product->name, product->brand, product->category, product->quantity, product->price);
    printf("|-------|----------------------------------------------------|---------------------------|---------------------------|----------|-----------|\n");
}