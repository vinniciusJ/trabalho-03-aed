//
// Created by gedt on 11/04/2024.
//
#include <stdio.h>

#include "headers/product-view.h"

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

double input_new_price(int * key){
    double price = 0.0;

    printf("Digite o codigo do produto: ");
    scanf("%d", key);

    printf("Novo preco (R$): ");
    scanf("%lf", &price);

    return price;
}

int input_code(){
    int code = 0;

    printf("Digite o codigo do produto: ");
    scanf("%d", &code);

    return code;
}

int input_new_quantity(int * key){
    int quantity = 0;

    printf("Digite o codigo do produto: ");
    scanf("%d", key);

    printf("Digite o estoque do produto: ");
    scanf("%d", &quantity);

    return quantity;
}

void input_string(char * label, char * value){
    printf("%s", label);
    scanf("%[^\n]%*c", value);
}

void show_products_header(){
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("|%-6s | %-50s | %-25s | %-25s | %-8s | %-10s|\n", "Codigo", "Nome", "Marca", "Categoria", "Estoque", "Valor");
    printf("|-------|----------------------------------------------------|---------------------------|---------------------------|----------|-----------|\n");
}

void show_product(Product * product){
    printf("|%-6d | %-50s | %-25s | %-25s | %-8d | %-10.2lf|\n", product->code, product->name, product->brand, product->category, product->quantity, product->price);
    printf("|-------|----------------------------------------------------|---------------------------|---------------------------|----------|-----------|\n");
}