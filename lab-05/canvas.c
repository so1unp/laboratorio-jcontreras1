#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> //O_COSAS
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>



#define HEIGHT  25  // Altura en caracteres de la pizarra
#define WIDTH   25  // Ancho en caracteres de la pizarra

struct canvas {
	char canvas[HEIGHT*WIDTH];
};

typedef struct canvas canvas_t;

void usage(char *argv[])
{
	fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
	fprintf(stderr, "Comandos:\n");
	fprintf(stderr, "\t-w canvas mensaje x y [h|v]: escribe el mensaje en el canvas indicado en la posición (x,y) de manera [h]orizontal o [v]ertical.\n");
	fprintf(stderr, "\t-p canvas: imprime el canvas indicado.\n");
	fprintf(stderr, "\t-c canvas: crea un canvas con el nombre indicado.\n");
	fprintf(stderr, "\t-d canvas: elimina el canvas indicado.\n");
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

	char option = argv[1][1];
	int fd; // File descriptor
	canvas_t *canvas; //Est. Canvas
	//i, j; //Para iterar sobre mapa

	switch(option) {
		case 'w':
			printf("Escribe %s en el canvas %s en la posición (%d, %d).\n", argv[3], argv[2], atoi(argv[4]), atoi(argv[5]));
			if (argc != 7){
            	usage(argv);
            	exit(EXIT_FAILURE);
        	}

			fd = shm_open(argv[2], O_RDWR, 0666);
    		size_t i;
			int x, y;

    		if (fd == -1){
        		perror("shm_open");
        		exit(EXIT_FAILURE);
    		}

    		canvas = mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    		if (canvas == MAP_FAILED){
        		perror("mmap");
        		close(fd);
		        exit(EXIT_FAILURE);
    		}

    		size_t len = strlen(argv[3]);

			x = atoi(argv[4]);
			y = atoi(argv[5]);

    		if (argv[6][0] == 'h'){
        		if (x >= 0 && x < HEIGHT && y >= 0 && y + len <= WIDTH){
            		for (i = 0; i < len; i++){
                		canvas->canvas[x * WIDTH + y + i] = argv[3][i];
					}
				}else{
            		fprintf(stderr, "Error: coordenadas fuera de rango.\n");
				}
			}else if (argv[6][0] == 'v'){
        		if (x >= 0 && x + len < HEIGHT && y >= 0 && y < WIDTH){
            		for (i = 0; i < len; i++){
                		canvas->canvas[(x + i) * WIDTH + y] = argv[3][i];
					}
				}else{
            		fprintf(stderr, "Error: coordenadas fuera de rango.\n");
				}
			}else{
        		fprintf(stderr, "Error: dirección desconocida '%c'. Use 'h' para horizontal o 'v' para vertical.\n", argv[6][0]);
			}

   			munmap(canvas, sizeof(canvas_t));
    		close(fd);
			
			break;
		case 'p':
			printf("Imprime canvas.\n");
			if (argc != 3){
            	usage(argv);
            	exit(EXIT_FAILURE);
        	}
			
			fd = shm_open(argv[2], O_RDONLY, 0666);

    		if (fd == -1){
        		perror("shm_open");
        		exit(EXIT_FAILURE);
    		}

    		canvas = mmap(NULL, sizeof(canvas_t), PROT_READ, MAP_SHARED, fd, 0);

    		if (canvas == MAP_FAILED){
        		perror("mmap");
        		close(fd);
        		exit(EXIT_FAILURE);
    		}
		int j;
    		for (i = 0; i < HEIGHT; i++){
        		for (j = 0; j < WIDTH; j++){
					putchar(canvas->canvas[i * WIDTH + j]);
    			}
        		putchar('\n'); //salto
    		}

    		munmap(canvas, sizeof(canvas_t));
    		close(fd);
			break;		
		case 'c':
			printf("Crea canvas.\n");
			if (argc != 3){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			
			int fd = shm_open(argv[2], O_CREAT | O_RDWR, 0666);
    		if (fd < 0){
        		perror("shm_open");
        		exit(EXIT_FAILURE);
    		}

    		if (ftruncate(fd, sizeof(canvas_t)) == -1){
        		perror("ftruncate");
        		exit(EXIT_FAILURE);
			}

    		canvas = mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

   	 		if (canvas == MAP_FAILED){
        		perror("mmap");
        		close(fd);
        		exit(EXIT_FAILURE);
			}

    		memset(canvas->canvas, ' ', sizeof(canvas->canvas));
    		munmap(canvas, sizeof(canvas_t));
    		close(fd);
			break;
		case 'd':
			printf("Borra canvas.\n");
			if (argc != 3){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			if (shm_unlink(argv[2]) < 0){
        		perror("shm_unlink");
        		exit(EXIT_FAILURE);
    		}
			break;
		case 'h':
			usage(argv);
			break;
		default:
			fprintf(stderr, "Comando desconocido\n");
			exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}