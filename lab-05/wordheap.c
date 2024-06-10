#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h> //shm
#include <sys/stat.h>
#include <unistd.h> //close




#define ITEMS       15
#define MAX_WORD    50

struct wordheap {
    int free;
    int items;
    int max_word;
    char heap[ITEMS][MAX_WORD];
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
};

typedef struct wordheap wordheap_t;

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w pila palabra: agrega palabra en la pila\n");
    fprintf(stderr, "\t-r pila pos: elimina la palabra de la pila.\n");
    fprintf(stderr, "\t-p pila: imprime la pila de palabras.\n");
    fprintf(stderr, "\t-c pila: crea una zona de memoria compartida con el nombre indicado donde almacena la pila.\n");
    fprintf(stderr, "\t-d pila: elimina la pila indicada.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    int i;      
    int shm_fd;  //fd compartido
    wordheap_t *wordheap;     // Pila
    char option = argv[1][1];

    switch(option) {
        case 'w':
            shm_fd = shm_open(argv[2], O_RDWR, 0666);
            if (shm_fd == -1) {
                fprintf(stderr, "Error al abrir la memoria compartida.\n");
                exit(EXIT_FAILURE);
            }
            wordheap = mmap(NULL, sizeof(wordheap_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            if (wordheap == MAP_FAILED) {
                fprintf(stderr, "Error al mapear la memoria compartida.\n");
                exit(EXIT_FAILURE);
            }
            if(wordheap->free == 0) {
                printf("Pila llena.\n");
                break;
            }
            sem_wait(&wordheap->empty);
            pthread_mutex_lock(&wordheap->mutex);
            strcpy(wordheap->heap[wordheap->items], argv[3]);
            wordheap->items++; 
            wordheap->free--;
            pthread_mutex_unlock(&wordheap->mutex);
            sem_post(&wordheap->full);
            printf("Palabra %s agregada a la pila %s.\n", argv[3], argv[2]);
            break;
        case 'r':
                shm_fd = shm_open(argv[2], O_RDWR, 0666);
                if (shm_fd == -1) {
                    fprintf(stderr, "Error al abrir la memoria compartida.\n");
                    exit(EXIT_FAILURE);
                }
                wordheap = mmap(NULL, sizeof(wordheap_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                if (wordheap == MAP_FAILED) {
                    fprintf(stderr, "Error al mapear la memoria compartida.\n");
                    exit(EXIT_FAILURE);
                }
                if(wordheap->items == 0) {
                    printf("Pila vacía.\n");
                    break;
                }
                sem_wait(&wordheap->full);
                pthread_mutex_lock(&wordheap->mutex);
                wordheap->items--;
                wordheap->free++;
                pthread_mutex_unlock(&wordheap->mutex);
                sem_post(&wordheap->empty);
                printf("Palabra eliminada de la pila %s.\n", argv[2]);
            break;
        case 'p':
                shm_fd = shm_open(argv[2], O_RDWR, 0666);
                if (shm_fd == -1) {
                    fprintf(stderr, "Error al abrir la memoria compartida.\n");
                    exit(EXIT_FAILURE);
                }
                wordheap = mmap(NULL, sizeof(wordheap_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                if (wordheap == MAP_FAILED) {
                    fprintf(stderr, "Error al mapear la memoria compartida.\n");
                    exit(EXIT_FAILURE);
                }
                if(wordheap->items == 0) {
                    printf("Pila vacía.\n");
                    break;
                }
                pthread_mutex_lock(&wordheap->mutex);
                if(wordheap->items == 0) {
                    printf("Pila vacía.\n");
                    pthread_mutex_unlock(&wordheap->mutex);
                    break;
                }
                for (i = 0; i < wordheap->items; i++) {
                    printf("%s\n", wordheap->heap[i]);
                }
                pthread_mutex_unlock(&wordheap->mutex);
            break;
        case 'c':
                shm_fd = shm_open(argv[2], O_CREAT | O_RDWR, 0666);
                if (shm_fd == -1) {
                    fprintf(stderr, "Error al crear la memoria compartida.\n");
                    exit(EXIT_FAILURE);
                }
                if (ftruncate(shm_fd, sizeof(wordheap_t)) == -1){
                    perror("ftruncate");
                    exit(EXIT_FAILURE);
                }
                wordheap = mmap(NULL, sizeof(wordheap_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                if (wordheap == MAP_FAILED) {
                    fprintf(stderr, "Error al mapear la memoria compartida.\n");
                    exit(EXIT_FAILURE);
                }
                // printf("llega aca\n");
                wordheap->free = ITEMS - 1;
                wordheap->items = 0;
                // printf("llega aca\n");
                wordheap->max_word = MAX_WORD; 
                pthread_mutex_init(&wordheap->mutex, NULL);
                sem_init(&wordheap->full, 1, 0);
                sem_init(&wordheap->empty, 1, ITEMS);
                close(shm_fd);
            break;
        case 'd':
            shm_fd = shm_open(argv[2], O_RDWR, 0666);
            if (shm_fd == -1) {
                fprintf(stderr, "Error al abrir la memoria compartida.\n");
                exit(EXIT_FAILURE);
            }
            wordheap = mmap(NULL, sizeof(wordheap_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            if (wordheap == MAP_FAILED) {
                fprintf(stderr, "Error al mapear la memoria compartida.\n");
                exit(EXIT_FAILURE);
            }
            //Eliminar primero los mutex y semaforos
            pthread_mutex_destroy(&wordheap->mutex);
            sem_destroy(&wordheap->full);
            sem_destroy(&wordheap->empty);
            shm_unlink(argv[2]);
            printf("Pila %s eliminada.\n", argv[2]);
            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "-%c: opción desconocida.\n", option);
            exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
