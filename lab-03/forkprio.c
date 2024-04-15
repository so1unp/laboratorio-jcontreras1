#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_HIJOS 25
#define MIN_HIJOS 1
#define PRIORIDAD_MINIMA 20

#define MAX_SEGS 10
#define MIN_SEGS 0


pid_t hijos[MAX_HIJOS];     //hijos
int nHijos;                 //cantidad de hijos

int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

/**MAta los hijos mediante ctrlC*/
void killSons(int signal){
    if(signal == SIGINT){
        int i;
        for(i = 0; i < nHijos; i++){
            kill(hijos[i], SIGTERM);
        }
        sleep(1);
        exit(EXIT_SUCCESS);
    }
}

// Handler para la señal de alarma
void alarm_handler(int signal) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    switch (signal)
	{
        case SIGINT: 
        //No hacer nada, porque sería propiedad del padre.
        break;
	case SIGTERM:
		printf("%d: Mi trabajo aquí ya está hecho...\n", getpid());
        exit(EXIT_SUCCESS);
		break;
	case SIGUSR1:
		//Imprimi algo de info
        
       	printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, 0), usage.ru_utime.tv_sec + usage.ru_stime.tv_sec);
       	// printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, 0), 1000000000000);
		break;
	case SIGUSR2:
		//Bajate un cacho la prior
		nice(1);
		break;
	default:
		printf("%d: Mi trabajo aquí ya está hecho\n", getpid());
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

    nHijos = atoi(argv[1]);
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
    
    int indiceHijo;
    int auxPrioridad = 0;

    for(indiceHijo = 0; indiceHijo < nHijos; indiceHijo++){
        if(prioridad){
            if(++auxPrioridad > PRIORIDAD_MINIMA){
                auxPrioridad = PRIORIDAD_MINIMA;
            }
        }
        if((hijos[indiceHijo] = fork()) == 0){
            setpriority(PRIO_PROCESS, 0, auxPrioridad);
            signal(SIGUSR1, alarm_handler);
            signal(SIGUSR2, alarm_handler);
            signal(SIGTERM, alarm_handler);
            signal(SIGINT, alarm_handler);
            busywork();
            exit(EXIT_SUCCESS);              
        }else{
            signal(SIGINT, killSons);             
        }
    }
    
	int i, j;
	/**Tiempo determinado o forever*/
	if(segundosEjecucion == 0){
		for(;;){
			sleep(1);
			for(j = 0; j < nHijos; j++){
				kill(hijos[j], SIGUSR1);
			}
		}
	}else{
		for(i = 0; i < segundosEjecucion; i++){
			sleep(1);
			for(j = 0; j < nHijos; j++){
				kill(hijos[j], SIGUSR1);
			}
		}
	}


    for (indiceHijo = 0; indiceHijo < nHijos; indiceHijo++) {
        kill(hijos[indiceHijo], SIGTERM);
        waitpid(hijos[indiceHijo], 0, 0);
    }
    printf("Todos los hijos terminaron\n");
    exit(EXIT_SUCCESS);
}
