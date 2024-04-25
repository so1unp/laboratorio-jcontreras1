#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int tiempoMaximoSleep;
void lanzarHilos(int count);

int main(int argc, char* argv[])
{

    int count;
    
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <hilos> <sleep>\n", argv[0]);
        fprintf(stderr, "\t<hilos>           Cantidad de hilos a generar.\n");
        fprintf(stderr, "\t<sleep>           Cantidad máxima de tiempo que duermen.\n");
        exit(EXIT_FAILURE);
    }

    count = atoi(argv[1]);
    tiempoMaximoSleep = atoi(argv[2]);

    if (count <= 0) {
        fprintf(stderr, "La cantidad de hilos debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if (tiempoMaximoSleep <= 0) {
        fprintf(stderr, "La cantidad de tiempo que duermen los hilos debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    lanzarHilos(count);    
    exit(EXIT_SUCCESS);
    // pthread_exit(NULL);
    // return 0;
}

void *descansar(void *args){
    
    /*Todo para generar un aleatorio*/
    unsigned short seed[3];
    seed[0] = (unsigned short)time(NULL);
    seed[1] = pthread_self();
    seed[2] = seed[1] + 1;
    int numeroAleatorio = 1 + (int)(nrand48(seed) % tiempoMaximoSleep);
    
    fprintf(stdout, "[%d]\tVoy a dormir %d\n", (int)pthread_self(), numeroAleatorio);
    sleep(numeroAleatorio);
    fprintf(stdout, "[%d]\tTerminé de dormir %d\n", (int)pthread_self(), numeroAleatorio);
    
    pthread_exit(NULL);
}

void lanzarHilos(int count) 
{
    int j, i, nuevoHilo;
    pthread_t hilos[count];

    for (j = 0; j < count; j++) {
        nuevoHilo = pthread_create(&hilos[j], NULL, descansar, NULL);
        if(nuevoHilo != 0){
            perror("thread");
            exit(0);
        }
    }

    for(i = 0; i < count; i++){
       pthread_join(hilos[i], NULL);
    }

}

