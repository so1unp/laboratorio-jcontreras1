#include <stdio.h>
#include <stdlib.h>

/*
Implementar en el programa sim.c un simulador de paginación y memoria virtual, 
que simule una computadora con 
64 KiB de memoria virtual, 
memoria física de 32 KiB,
 un tamaño de las páginas de 4 KiB 
 y 64 KiB de espacio en almacenamiento secundario 
 para páginas que se desalojen. El parámetro que debe aceptar el programa es el tipo 
 de algoritmo de reemplazo de páginas: -f (FIFO) o -l (LRU).

El simulador debe leer desde la entrada estándar un identificador del proceso y 
el número de página que modifica, uno por línea. A medida que va leyendo, realiza 
la asignación en memoria, si es posible. Debe finalizar al leer un EOF.

Por ejemplo, la siguiente entrada indica que el proceso 1 accede a las páginas 3 y 9,
 y el proceso 2 a la página 1.

1
3
1
9
1
3
2
1
Una vez finalizada la ejecución el simulador debe imprimir:

La tabla de páginas de cada proceso. El formato debe ser el siguiente (usando el ejemplo anterior):
Proceso 1: - - 1 - - - - - 2 - - - - - - - -
Proceso 2: 3 - - - - - - - - - - - - - - - -
La memoria física. Mismo formato que las tablas de páginas, pero cada entrada debe ser PID.PAGINA. Para el ejemplo anterior:
1.3 1.9 2.1 - - - - -
Indica que en el primer marco se almaceno la tercer página del proceso 1.
El almacenamiento secundario. Mismo formato que la memoría física. Para el ejemplo anterior:
- - - - - - - - - - - - - - - - -
*/


#define MEMORIA_VIRTUAL 64
#define MEMORIA_FISICA 32
#define TAM_PAGINA 4
#define MEMORIA_SECUNDARIA 64
#define PROCESOS 20

typedef struct {
    int numero;
    int pid;
} Pagina; 
//fisica

typedef struct {
    int pid;
    char memoria; //f fisica, d disco
    int existe;
    int paginas[MEMORIA_VIRTUAL/TAM_PAGINA]; //tabla de páginas abiertas
} Proceso;


Proceso tablaProcesos[PROCESOS];
Pagina mem_fisica[MEMORIA_FISICA/TAM_PAGINA];
Pagina mem_secundaria[MEMORIA_SECUNDARIA/TAM_PAGINA];


void vaciarTablaProcesos(){
    int i;
    for(i = 0; i < PROCESOS; i++){
        tablaProcesos[i].existe = -1;
        tablaProcesos[i].pid = -1;
        int j;
        for(j = 0; j < MEMORIA_VIRTUAL/TAM_PAGINA; j++){
            tablaProcesos[i].paginas[j] = -1;
        }
    }
}


void usage(char *programa){
    printf("Uso: %s -f | -l \n", programa);
    exit(EXIT_FAILURE);
}
int main(int argc, char* argv[]){

    if(argc != 2){
        usage(argv[0]);
    }

    if (argv[1][0] != '-') {
        usage(argv[0]);
    }

    //char modo = argv[1][1];
    int posActualMemoria = 0; //ESTO ES EL INDICE QUE VA A MANEJAR LA MEMORIA RAM
    int i, j;
    //Vaciar memoria fisica
    for (i = 0; i < MEMORIA_FISICA/TAM_PAGINA; ++i) {
        mem_fisica[i].numero = -1;
        mem_fisica[i].pid = -1;
    }
    //Vaciar disco
    for (i = 0; i < MEMORIA_SECUNDARIA/TAM_PAGINA; ++i) {
        mem_secundaria[i].numero = -1;
        mem_secundaria[i].pid = -1;
    }
    vaciarTablaProcesos();
    int pid;
    int pagina;
    
    while (scanf("%d\n%d", &pid, &pagina) != EOF) {
    
        if(pid < 0 || pid > PROCESOS){
            printf("El pid tiene que estar entre 1 y %d\n", PROCESOS);
            exit(EXIT_FAILURE);
        }
        //Existe proceso?
        if(tablaProcesos[pid - 1].existe == -1){
            //no existe -> crea
            printf("No existe el proceso. Creando proceso %d\n", pid);
            tablaProcesos[pid - 1].existe = 1;
            tablaProcesos[pid - 1].pid = pid;
            tablaProcesos[pid - 1].memoria = 'f';
        }
       
        printf("Proceso %d accede a la pagina %d\n", pid, pagina);
        printf("Posicion actual de memoria: %d\n", posActualMemoria);
        printf("Posicion actual de memoria: %d\n", posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA));
       // tablaProcesos[pid - 1].paginas[pagina - 1] = posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA);
        
        if(mem_fisica[posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA)].numero != -1){
            //pagina ya esta en memoria, debo reemplazar
            j = 2;
            i = j;
            continue;
        }
        
        Pagina pag;
        pag.numero = pagina - 1;
        pag.pid = pid;
        mem_fisica[posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA)] = pag;

    posActualMemoria++;
    }
    /* Imprime tabla de procesos */
    for ( i = 0; i < PROCESOS; i++){
        if(tablaProcesos[i].existe > -1){
            //Imprime el proceso si existe
            printf("Proceso %d: ", tablaProcesos[i].pid);
            for (j = 0; j < MEMORIA_VIRTUAL/TAM_PAGINA; j++) {
                if (tablaProcesos[i].paginas[j] != -1) {
                    printf("%d ", tablaProcesos[i].paginas[j]);
                } else {
                    printf("- ");
                }
            }
            printf("\n");
        }

    }

    /* Imprime memoria fisica */
    printf("Memoria fisica: ");
    for (i = 0; i < MEMORIA_FISICA/TAM_PAGINA; i++) {
        if (mem_fisica[i].numero != -1) {
            printf("%d.%d ", mem_fisica[i].pid, mem_fisica[i].numero);
        } else {
            printf("- ");
        }
    }
    printf("\n");

    //Imprime memoria secundaria
    printf("Memoria secundaria: ");
    for (i = 0; i < MEMORIA_SECUNDARIA/TAM_PAGINA; i++) {
        if (mem_secundaria[i].numero != -1) {
            printf("%d.%d ", mem_secundaria[i].pid, mem_secundaria[i].numero);
        } else {
            printf("- ");
        }
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
