#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int cantidadNumeros = argc -1;
    int i;
    int sumaTotal = 0;
    for( i = 0; i <= cantidadNumeros; i++){
        sumaTotal += atoi(argv[i]);
    }
    printf("La suma es %d\n", sumaTotal);
    exit(EXIT_SUCCESS);
}
