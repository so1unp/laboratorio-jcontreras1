/*
 * Invoca la llamada al sistema answer().
 */
#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char *argv[])
{
    printf(1, "Antes del fork, hay %d procesos.\n", pscnt());
    int pid = fork(); // Crear un nuevo proceso
    if (pid < 0) { // Error al hacer fork
        printf(2, "Fork fallido\n");
        exit();
    }
    if(pid != 0){
        //lo imprimo desde el padre
        printf(1, "Luego del fork, hay %d procesos.\n", pscnt());
    }
    kill(pid);
    sleep(1);
    exit();
}
