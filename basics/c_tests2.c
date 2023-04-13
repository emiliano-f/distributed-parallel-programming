#include<stdio.h>
#include<stdlib.h>

struct MyStruct
{
    char c;
    int x;
    double d;
};
/*
struct MyStruct *createStruct(char cparam, int xparam, double dparam)
{
    struct MyStruct temps;
    temps.c = cparam;
    temps.x = xparam;
    temps.d = dparam;
    return &temps;
}*/

// Modificación sugerida
struct MyStruct *createStruct(char cparam, int xparam, double dparam)
{
    struct MyStruct *temps = malloc(sizeof(struct MyStruct));
    temps->c = cparam;
    temps->x = xparam;
    temps->d = dparam;
    return temps;
}

int main(void)
{
    struct MyStruct *r;
    r = createStruct('c', 123, 456.789);
    struct MyStruct s = *r;
    // Esto no sirve porque la variable local de la función se saca de la pila
    printf("Member c has a value of %c\n", s.c);
    printf("Member x has a value of %d\n", s.x);
    printf("Member d has a value of %f\n", s.d);
    
    // google sugiere crear utilizar memoria dinamica para que la estructura
    // permanezca en el heap y no en la pila
}
