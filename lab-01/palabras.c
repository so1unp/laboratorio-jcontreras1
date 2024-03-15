#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    int caracter;
    while((caracter = getchar()) != EOF){
        if(caracter == ' '){
            putchar('\n');
        }else{
            putchar(caracter);
        }
    }

    // Versión PRO
    // char palabras[191];
    // while(scanf("%s", palabras)){
    //     printf("%s\n", palabras);
    // }

    exit(EXIT_SUCCESS);
}
