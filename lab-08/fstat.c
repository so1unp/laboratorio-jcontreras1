#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

void info(const char *archivo) {
    struct stat file_stat;

    if (stat(archivo, &file_stat) == -1) {
        perror("No se puede abrir el archivo");
        return;
    }

    printf("ID del usuario propietario: %d\n", file_stat.st_uid);
    printf("Tipo de archivo: %o\n", file_stat.st_mode & S_IFMT);
    printf("Número de i-nodo: %ld\n", file_stat.st_ino);
    printf("Tamaño en bytes: %ld\n", file_stat.st_size);
    printf("Número de bloques en disco: %ld\n", file_stat.st_blocks);
    printf("Tiempo de última modificación: %s", ctime(&file_stat.st_mtime));
    printf("Tiempo de último acceso: %s", ctime(&file_stat.st_atime));
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <ruta del archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    info(argv[1]);

    exit(EXIT_SUCCESS);
}