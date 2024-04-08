#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()

#define MAX_HIJOS 10
#define MIN_HIJOS 1

#define MAX_SEGS 10
#define MIN_SEGS 1

int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, char *argv[])
{
    if(argc < 4){
        printf("Uso: forkprio x y z. X: numero de procesos hijos a crear. Y: El segundo parámetro es el número de segundos que deben ejecutar. Z: puede tomar valores 1 o 0 para reducir o no las prioridades. \n");
        exit(EXIT_SUCCESS);
    }

    int nHijos = atoi(argv[1]);
    int segundosEjecucion = atoi(argv[2]);
    int prioridad = atoi(argv[3]);

    /* Validación de parametros */
    if(nHijos < MIN_HIJOS || nHijos > MAX_HIJOS){
        printf("Cantidad de hijos (primer parametro), debe estar entre: %d y %d\n", MIN_HIJOS, MAX_HIJOS);
        exit(EXIT_SUCCESS);
    }

    if(segundosEjecucion < MIN_SEGS || segundosEjecucion > MAX_SEGS){
        printf("Cantidad de segundos de ejecucion (segundo parametro), debe estar entre: %d y %d\n", MIN_SEGS, MAX_SEGS);
        exit(EXIT_SUCCESS);
    }
    if(prioridad != 0 && prioridad != 1){
        printf("Los valores posibles para el tercer parametro son 1 y 0 para reducir (o no), la prioridad.\n");
        exit(EXIT_SUCCESS);
    }

    char *txtReduccionPrioridad = (prioridad == 0) ? "Sin" : "Con"; 
    printf("Creando %d hijos, con una ejecucion de %d segundos. %s reducción de prioridad.\n", nHijos, segundosEjecucion, txtReduccionPrioridad); 


    exit(EXIT_SUCCESS);
}
