#include <stdio.h>

int main(void) {
    int x = 33;
    printf("%i\n", x);
    int *p;
    printf("%i\n", p);
    p = &x;
    printf("%i\n", p);
    *p = 22;
    printf("%i\n", x);
    
    int arr[5] = {9,8,7,6,5};
    p = arr;
    for (int i = 0; i<8; i++){
        printf("%d\n",*(p+i));
    }

    // muestra direcciones de memoria de cada string
    char *array_punt[] = {"FirstSentence", "SecondSentence", "ThirdSentence"};
    for (int i = 0; i<3; i++){
        printf("%s -> %p\n", array_punt[i], (void *) array_punt[i]);
    }
    
    // muestra direcciones de memoria del array puntero y lo que contiene
    // lo que contiene deberia coincidir con lo impreso en el bucle for anterior
    for (int i = 0; i<3; i++) {
        void * punteros = array_punt;
        printf("%p\n", (void *) (array_punt+i) ); // -> %p\n", punteros+i, (void *) &(punteros+i));        
        //printf("%p -> %s\n", (void *)&array_punt[i], array_punt[i]);
    }

}
