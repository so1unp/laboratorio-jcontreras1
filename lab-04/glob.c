#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Variable global que incrementan los hilos.
static long glob = 0;
sem_t mutex;
void increment_glob(int *loops)
{
    int loc, j;
    // incrementa glob
    for (j = 0; j < *loops; j++) {
        sem_wait(&mutex);
        loc = glob;
        loc++;
        glob = loc;
        sem_post(&mutex);
    }
}

int main(int argc, char *argv[])
{
    int loops, i;
    int cantidadHilos = 2;
    pthread_t hilos[cantidadHilos];

    // Controla numero de argumentos.
    if (argc != 2) {
        fprintf(stderr, "Uso: %s ciclos\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    loops = atoi(argv[1]);

    // Verifica argumentos.
    if (loops < 1) {
        fprintf(stderr, "Error: ciclos debe ser mayor a cero.\n");
        exit(EXIT_FAILURE);
    }

    printf("Valor antes: %ld\n", glob);

    if (sem_init(&mutex, 0, 1) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    
    // Crea los hilos
    for (i = 0; i < cantidadHilos; i++) {
        if (pthread_create(&hilos[i], NULL, (void *) increment_glob, &loops) != 0) {
            perror("phtread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Espera la finalizacion de la ejecución
    for (i = 0; i < cantidadHilos; i++) {
        if (pthread_join(hilos[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    sem_destroy(&mutex);
    printf("Valor despues: %ld\n", glob);

    exit(EXIT_SUCCESS);
}
