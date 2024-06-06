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


#define MEMORIA_VIRTUAL 32
#define MEMORIA_FISICA 16
#define TAM_PAGINA 4
#define MEMORIA_SECUNDARIA 32
#define PROCESOS 20

typedef struct {
	int numero;
	int pid;
	int timestamp;
} Pagina; 
//fisica

typedef struct {
	int pid;
	char memoria[MEMORIA_VIRTUAL/TAM_PAGINA]; //f fisica, d disco
	int paginas[MEMORIA_VIRTUAL/TAM_PAGINA]; //tabla de páginas abiertas
} Proceso;

int falsoTimestamp = 0;
int posActualMemoria = 0; //ESTO ES EL INDICE QUE VA A MANEJAR LA MEMORIA RAM
int posActualDisco = 0;
Proceso tablaProcesos[PROCESOS];
Pagina mem_fisica[MEMORIA_FISICA/TAM_PAGINA];
Pagina mem_secundaria[MEMORIA_SECUNDARIA/TAM_PAGINA];


void vaciarTablaProcesos(){
	int i;
	for(i = 0; i < PROCESOS; i++){
		tablaProcesos[i].pid = -1;
		int j;
		for(j = 0; j < MEMORIA_VIRTUAL/TAM_PAGINA; j++){
			tablaProcesos[i].paginas[j] = -1;
			tablaProcesos[i].memoria[j] = '-';
		}
	}
}

void vaciarDisco(){
	int i;
	for(i = 0; i < MEMORIA_SECUNDARIA/TAM_PAGINA; i++){
		mem_secundaria[i].numero = -1;
		mem_secundaria[i].pid = -1;
	}
}

void vaciarMemoria(){
	int i;
	for(i = 0; i < MEMORIA_FISICA/TAM_PAGINA; i++){
		mem_fisica[i].numero = -1;
		mem_fisica[i].pid = -1;
		mem_fisica[i].timestamp = -1;
	}
}

void imprimirMemoriaFisica(){
	/* Imprime memoria fisica */
	int i;
	printf("Memoria fisica: ");
	for (i = 0; i < MEMORIA_FISICA/TAM_PAGINA; i++) {
		if (mem_fisica[i].numero != -1) {
			printf("%d.%d.%d ", mem_fisica[i].pid, mem_fisica[i].numero, mem_fisica[i].timestamp);
		} else {
			printf("- ");
		}
	}
	printf("\n");
}

void imprimirMemoriaSecundaria(){
	//Imprime memoria secundaria
	printf("Memoria secundaria: ");
	int i;
	for (i = 0; i < MEMORIA_SECUNDARIA/TAM_PAGINA; i++) {
		if (mem_secundaria[i].numero != -1) {
			printf("%d.%d ", mem_secundaria[i].pid, mem_secundaria[i].numero);
		} else {
			printf("- ");
		}
	}
	printf("\n");
}

void imprimirTablaProcesos(){
	/* Imprime tabla de procesos y su uso de memoria */
	int i,j;
	for(i = 0; i < PROCESOS; i++){
		if(tablaProcesos[i].pid != -1){
			printf("Proceso %d: ", i+1);
			for(j = 0; j < MEMORIA_VIRTUAL/TAM_PAGINA; j++){
				if(tablaProcesos[i].paginas[j] != -1){
					printf("[%d,%c] ", tablaProcesos[i].paginas[j], tablaProcesos[i].memoria[j]);
				}else{
					printf("- ");
				}
			}
			printf("\n");
		}
	}
}

/*
            _                 _         _____                                _                   
     /\    | |               | |       |  __ \                              | |                  
    /  \   | |  __ _       __| |  ___  | |__) | ___   ___  _ __ ___   _ __  | |  __ _  ____ ___  
   / /\ \  | | / _` |     / _` | / _ \ |  _  / / _ \ / _ \| '_ ` _ \ | '_ \ | | / _` ||_  // _ \ 
  / ____ \ | || (_| | _  | (_| ||  __/ | | \ \|  __/|  __/| | | | | || |_) || || (_| | / /| (_) |
 /_/    \_\|_| \__, |(_)  \__,_| \___| |_|  \_\\___| \___||_| |_| |_|| .__/ |_| \__,_|/___|\___/ 
                __/ |                                                | |                         
               |___/                                                 |_|                                                                                                              |_|                        
*/

void fifo(int pid, int pagina){
	// Verificar si hay espacio en memoria física para la página
	if(posActualMemoria >= MEMORIA_FISICA/TAM_PAGINA){
		// La memoria física está llena, debería insertarse en disco
		printf("Memoria llena, reemplazo %d\n", posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA));
		if(posActualDisco >= MEMORIA_SECUNDARIA/TAM_PAGINA){
			printf("Disco lleno, capo.\n");
			exit(EXIT_FAILURE);
		}
		// Mover la página de memoria física a disco
		mem_secundaria[posActualDisco] = mem_fisica[posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA)];
		// Buscar en la tabla de procesos y cambiar la página a disco
		int i;
		for(i = 0; i < PROCESOS; i++){
		    if(tablaProcesos[i].pid != -1 && tablaProcesos[i].pid == mem_secundaria[posActualDisco].pid){
		        // Buscar la página en la tabla de páginas del proceso y cambiar su estado a disco
		        for(int j = 0; j < MEMORIA_VIRTUAL/TAM_PAGINA; j++) {
		            if(tablaProcesos[i].paginas[j] == mem_secundaria[posActualDisco].numero) {
		                tablaProcesos[i].memoria[j] = 'D';
		                break;
		            }
		        }
		        break;
		    }
		}
		posActualDisco++;
	}
	// Insertar la página en la memoria física
  	Pagina pag;
  	pag.numero = pagina;
  	pag.pid = pid;
  	mem_fisica[posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA)] = pag;
  	// Actualizar la tabla de procesos para reflejar la asignación de la página
  	tablaProcesos[pid - 1].paginas[pagina - 1] = pagina;
  	tablaProcesos[pid - 1].memoria[pagina - 1] = 'F';
}

void lru(int pid, int pagina){
	// Verificar si hay espacio en memoria física para la página
	int ubicacionMemoria = posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA);
	
	if(posActualMemoria >= MEMORIA_FISICA/TAM_PAGINA){
		// La memoria física está llena, debería insertarse en disco
		printf("Memoria llena, reemplazo %d usando LRU \n", posActualMemoria%(MEMORIA_FISICA/TAM_PAGINA));
		if(posActualDisco >= MEMORIA_SECUNDARIA/TAM_PAGINA){
			printf("Disco lleno, capo.\n");
			exit(EXIT_FAILURE);
		}
		// Mover la página de memoria física a disco
		int i, ubicacionMemoria = 0, timestampMasChico = mem_fisica[0].timestamp;
		//buscar el timestamp mas chico
		for(i = 0; i < MEMORIA_FISICA/TAM_PAGINA; i++){
			if(mem_fisica[i].timestamp < timestampMasChico){
				timestampMasChico = mem_fisica[i].timestamp;
				ubicacionMemoria = i;
			}
		}
		printf("Reemplazando %d\n", ubicacionMemoria);
		mem_secundaria[posActualDisco] = mem_fisica[ubicacionMemoria];
		// Buscar en la tabla de procesos y cambiar la página a disco
		for(i = 0; i < PROCESOS; i++){
		    if(tablaProcesos[i].pid != -1 && tablaProcesos[i].pid == mem_secundaria[posActualDisco].pid){
		        // Buscar la página en la tabla de páginas del proceso y cambiar su estado a disco
				int j;
		        for(j = 0; j < MEMORIA_VIRTUAL/TAM_PAGINA; j++) {
		            if(tablaProcesos[i].paginas[j] == mem_secundaria[posActualDisco].numero) {
		                tablaProcesos[i].memoria[j] = 'D';
		                break;
		            }
		        }
		        break;
		    }
		}
		posActualDisco++;
	}
	// Insertar la página en la memoria física
  	Pagina pag;
  	pag.numero = pagina;
  	pag.pid = pid;
	pag.timestamp = falsoTimestamp;
	printf("Aunque no lo creas, esta es la ubicacionMemoria %d\n", ubicacionMemoria);
	int i, timestampMasChico = mem_fisica[0].timestamp;
		//buscar el timestamp mas chico
		for(i = 0; i < MEMORIA_FISICA/TAM_PAGINA; i++){
			if(mem_fisica[i].timestamp < timestampMasChico){
				timestampMasChico = mem_fisica[i].timestamp;
				ubicacionMemoria = i;
			}
		}
  	mem_fisica[ubicacionMemoria] = pag;
  	// Actualizar la tabla de procesos para reflejar la asignación de la página
  	tablaProcesos[pid - 1].paginas[pagina - 1] = pagina;
  	tablaProcesos[pid - 1].memoria[pagina - 1] = 'F';
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

	vaciarMemoria();
	vaciarDisco();
	vaciarTablaProcesos();

	char modo = argv[1][1];

	int pid;
	int pagina;
	
			while (scanf("%d\n%d", &pid, &pagina) != EOF) {

				falsoTimestamp++;
	
				if(pid < 0 || pid > PROCESOS){
					printf("El pid tiene que estar entre 1 y %d\n", PROCESOS);
					exit(EXIT_FAILURE);
				}

				
				if(tablaProcesos[pid - 1].pid == -1){
        			// Si el proceso no existe, crearlo
        			printf("No existe el proceso. Creando proceso %d\n", pid);
        			tablaProcesos[pid - 1].pid = pid;
        			tablaProcesos[pid - 1].memoria[0] = 'F'; // Suponiendo que la primera página siempre se asigna a memoria física
			    }else{
					int paginaYaEnMemoria = 0;
					for(int i = 0; i < MEMORIA_FISICA/TAM_PAGINA; i++) {
					    if(mem_fisica[i].pid == pid && mem_fisica[i].numero == pagina) {
							mem_fisica[i].timestamp = falsoTimestamp;
					        printf("La página %d del proceso %d ya está en memoria física.\n", pagina, pid);
							imprimirMemoriaFisica();
					        paginaYaEnMemoria = 1;
					        break;
					    }
					}

					if(paginaYaEnMemoria) {
    					continue; // Pasar a la siguiente iteración del bucle
					}
				}

				switch(modo){
					case 'f':
						fifo(pid, pagina);
						break;
					case 'l':
						lru(pid, pagina);
						break;
					default:
						usage(argv[0]);
				}

				posActualMemoria++;
			}


	
	
	
	/* Imprime tabla de procesos */
	

	imprimirMemoriaFisica();
	imprimirMemoriaSecundaria();
	imprimirTablaProcesos();

	

	exit(EXIT_SUCCESS);
}
