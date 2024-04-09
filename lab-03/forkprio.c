#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define MAX_HIJOS 10
#define MIN_HIJOS 1

#define MAX_SEGS 10
#define MIN_SEGS 0

int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

// Handler para la señal de alarma
void alarm_handler(int signal) {
    switch (signal)
	{
	case SIGTERM:
		printf("%d: Mi trabajo aquí ya está hecho...\n", getpid());
        exit(EXIT_SUCCESS);
		break;
	case SIGUSR1:
		//Imprimi algo de info
       	printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, 0), 10000000000000);
		break;
	case SIGUSR2:
		//Bajate un cacho la prior
		nice(1);
		break;
	default:
		printf("%d: Mi trabajo aquí ya está hecho...\n", getpid());
        exit(EXIT_SUCCESS);
		break;
	}
}

int main(int argc, char *argv[])
{
    if(argc < 4){
        printf("Uso: forkprio <cant_hijos> <segundos_ejecucion_hijos> <reducir_prioridades [0-1]>.\n");
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

    /**Creación de hijos*/
    pid_t hijos[nHijos];
    int indiceHijo;
    int indiceTiempo;
   // int nuevoHijo;
    //Para utilizar los usos de tiempo
    //struct rusage usage;
    //Para calcular las diferencias de tiempo
    //struct timeval start, end;

    // Establecer el manejador de señal para SIGALRM

    for(indiceHijo = 0; indiceHijo < nHijos; indiceHijo++){
        if((hijos[indiceHijo] = fork()) == 0){
            signal(SIGUSR1, alarm_handler);
            signal(SIGUSR2, alarm_handler);
            signal(SIGTERM, alarm_handler);
            busywork();
            //getrusage(RUSAGE_SELF, &usage);
            //start = usage.ru_stime;
            /*Hijo*/
            
               //getrusage(RUSAGE_SELF, &usage);
               //end = usage.ru_stime;
               //long elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
               //printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, 0), elapsed_time);
                
            exit(EXIT_SUCCESS);
        }else{
 
             
        }
    }
    
	int i, j;
	/**Tiempo determinado o forever*/
	if(segundosEjecucion == 0){
		for(;;){
			sleep(1);
			for(j = 0; j < nHijos; j++){
				kill(hijos[j], SIGUSR1);
				if(prioridad)
					kill(hijos[j], SIGUSR2);
			}
		}
	}else{
		for(i = 0; i < segundosEjecucion; i++){
			sleep(1);
			for(j = 0; j < nHijos; j++){
				kill(hijos[j], SIGUSR1);
				if(prioridad)
					kill(hijos[j], SIGUSR2);
			}
		}
	}


    for (indiceHijo = 0; indiceHijo < nHijos; indiceHijo++) {
        kill(hijos[indiceHijo], SIGTERM);
    }
    for(indiceTiempo = 0; indiceTiempo < segundosEjecucion; indiceTiempo++){
        wait(NULL);
    }
   //printf("Proceso hijo terminó.\n");


    exit(EXIT_SUCCESS);
}
