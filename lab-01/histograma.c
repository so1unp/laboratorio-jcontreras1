#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    int cantdidadMaximaLetras = 20;
    int cantidad[cantdidadMaximaLetras];
    int auxLength = 0;
    int caracter;
    int i;
    for (i = 0; i <= cantdidadMaximaLetras; i++){
        cantidad[i] = 0;
    }
    
   while((caracter = getchar()) != EOF){
        if(caracter == ' ' || caracter == '\n'){
            //hay un separador
            ++cantidad[auxLength];
            auxLength = 0; 
        }else{
            ++auxLength;
            //reviso si excede la cantidad máxima de caracteres de este sistema
            if(auxLength > cantdidadMaximaLetras){
                printf("\nLa palabra mas larga permitida es de %d caracteres.\n", cantdidadMaximaLetras);
                exit(132);

            }
        }
    }

    //me mando una recorrida para ver cual es la longitud máxima. 
    // Asi no hago un histograma grande al divino botón
    int indicePalabraMasLarga = 0;
    for(i = 0; i < cantdidadMaximaLetras; i++){
        if(cantidad[i] > 0){
            indicePalabraMasLarga = i;
        }
    }
    int j;
    for (i = 1; i <= indicePalabraMasLarga; i++){
        //printf("Con %d, %d palabras\n", i,cantidad[i]);
        printf("%d ", i);
        for(j = 0; j < cantidad[i]; j++){
             printf("*");
        }
        printf("\n");

    }


    exit(EXIT_SUCCESS);
}
