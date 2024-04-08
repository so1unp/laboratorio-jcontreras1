#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void handleSignal(int signal){
    char msg[100];
    snprintf(msg, sizeof(msg), "%d: %s\n", signal, strsignal(signal));
    write(STDOUT_FILENO, msg, strlen(msg));
    fflush(stdout);
}

int main(void)
{
    /*Estructura de manejo de señales*/
    struct sigaction sa;
    sa.sa_handler = handleSignal;

    //Atrapamos todas las seniales de la 1 a la 31
    int indiceSenial;
    for(indiceSenial = 1; indiceSenial < NSIG; indiceSenial++){
        //signal(indiceSenial, handleSignal);
        sigaction(indiceSenial, &sa, NULL);
    }

    while(1){
        fflush(stdout);
        pause();        
    }
    
    exit(EXIT_SUCCESS);
}
