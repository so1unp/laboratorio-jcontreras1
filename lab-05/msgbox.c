#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>



void recibirMensaje(const char *colaNombre);
void crearColaMensaje(const char*);
void enviarMensaje(const char*, const char*);
void eliminarColaMensaje(const char *colaNombre);
void imrimirTodosLosMensajes(const char *colaNombre);
void infoColaMensajes(const char *colaNombre);
void escuchaActivaCola(const char *colaNombre);

#define USERNAME_MAXSIZE    15  // Máximo tamaño en caracteres del nombre del remitente.
#define TXT_SIZE            100 // Máximo tamaño del texto del mensaje.
#define BUFFER_SIZE         20  // tamaño del buffer de mensajes
#define MODO                0664

/**
 * Estructura del mensaje:
 * - sender: nombre del usuario que envió el mensaje.
 * - text: texto del mensaje.
 */
struct msg {
	char sender[USERNAME_MAXSIZE];
	char text[TXT_SIZE];
};

typedef struct msg msg_t;

/**
 * Imprime información acerca del uso del programa.
 */
void usage(char *argv[])
{
	fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
	fprintf(stderr, "Comandos:\n");
	fprintf(stderr, "\t-s queue mensaje: escribe el mensaje en queue.\n");
	fprintf(stderr, "\t-r queue: imprime el primer mensaje en queue.\n");
	fprintf(stderr, "\t-a queue: imprime todos los mensaje en queue.\n");
	fprintf(stderr, "\t-l queue: vigila por mensajes en queue.\n");
	fprintf(stderr, "\t-c queue: crea una cola de mensaje queue.\n");
	fprintf(stderr, "\t-d queue: elimina la cola de mensajes queue.\n");
	fprintf(stderr, "\t-h imprime este mensaje.\n");
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		usage(argv);
		exit(EXIT_FAILURE);
	}

	if (argv[1][0] != '-') {
		usage(argv);
		exit(EXIT_FAILURE);
	}

	char option = argv[1][1];

	switch(option) {
		case 's':
			if(argc < 4){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			enviarMensaje(argv[2], argv[3]);
			break;
		case 'r':
			if(argc < 3){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			printf("Recibe el primer mensaje en %s\n", argv[2]);
			recibirMensaje(argv[2]);
			break;
		case 'a':
			printf("Imprimer todos los mensajes en %s\n", argv[2]);
			imrimirTodosLosMensajes(argv[2]);
			break;
		case 'l':
			if(argc < 3){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			printf("Escucha indefinidamente por mensajes\n");
			escuchaActivaCola(argv[2]);
			break;
		case 'c':
			if(argc < 3){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			crearColaMensaje(argv[2]);   
			break;
		case 'd':
			if(argc < 3){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			printf("Borra la cola de mensajes %s\n", argv[2]);
			eliminarColaMensaje(argv[2]);   
			break;
		case 'h':
			usage(argv);
			break;
		case 'i':
			if(argc < 3){
				usage(argv);
				exit(EXIT_FAILURE);
			}
			printf("Información de la cola de mensajes %s\n", argv[2]);
			infoColaMensajes(argv[2]);   
			break;
		default:
			fprintf(stderr, "Comando desconocido: %s\n", argv[1]);
			exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}


void crearColaMensaje(const char *name)
{
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(msg_t);
	attr.mq_curmsgs = 0;

	mqd_t cola = mq_open(name, O_CREAT | O_RDWR, 0666, &attr);

	if (cola == (mqd_t)-1){
		perror("mq_open");
		exit(EXIT_FAILURE);
	}

	mq_close(cola);
}

void enviarMensaje(const char *colaNombre, const char *mensaje){
	/** Abrir y revisar la cola ( ͡° ͜ʖ ͡°) */
	mqd_t cola = mq_open(colaNombre, O_RDWR);
	if (cola == (mqd_t)-1){
		perror("mq_open");
		exit(EXIT_FAILURE);
	}

	msg_t msg;
	if (getlogin_r(msg.sender, USERNAME_MAXSIZE) != 0){
		perror("getlogin_r");
		mq_close(cola);
		exit(EXIT_FAILURE);
	}

	strncpy(msg.text, mensaje, TXT_SIZE - 1);
	msg.text[TXT_SIZE - 1] = '\0';
	if (mq_send(cola, (char*) &msg, sizeof(msg_t), 0) == -1){
		perror("mq_send");
		mq_close(cola);
		exit(EXIT_FAILURE);
	}

	mq_close(cola);
}

void recibirMensaje(const char *colaNombre){
	/** Abrir y revisar la cola ( ͡° ͜ʖ ͡°) */
	mqd_t cola = mq_open(colaNombre, O_RDONLY);
	if (cola == (mqd_t)-1){
		perror("mq_open");
		exit(EXIT_FAILURE);
	}

	msg_t msg;
	if (mq_receive(cola, (char *)&msg, sizeof(msg_t), NULL) == -1){
		perror("mq_receive");
		mq_close(cola);
		exit(EXIT_FAILURE);
	}

	printf("%s: %s\n", msg.sender, msg.text);
	mq_close(cola);
}

void eliminarColaMensaje(const char *colaNombre){
	 if (mq_unlink(colaNombre) == -1){
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
}

void imrimirTodosLosMensajes(const char *colaNombre){
	/** Abrir y revisar la cola ( ͡° ͜ʖ ͡°) */
	mqd_t cola = mq_open(colaNombre, O_RDONLY);
	if (cola == (mqd_t)-1){
		perror("mq_open");
		exit(EXIT_FAILURE);
	}

	struct mq_attr attr;
    int countMsgs;
	msg_t msg;

	mq_getattr(cola, &attr);
	countMsgs = attr.mq_curmsgs;

	while (countMsgs > 0) {
        if(mq_receive(cola, (char*) &msg, attr.mq_msgsize, NULL) == -1){
			perror("mq_receive");
			mq_close(cola);
			exit(EXIT_FAILURE);
		}
        printf("%s: %s\n", msg.sender, msg.text);
		countMsgs--;
    }
	mq_close(cola);
}

void infoColaMensajes(const char *colaNombre){
	/** Abrir y revisar la cola ( ͡° ͜ʖ ͡°) */
	mqd_t cola = mq_open(colaNombre, O_RDONLY);
	if (cola == (mqd_t)-1){
		perror("mq_open");
		exit(EXIT_FAILURE);
	}

	struct mq_attr attr;
	mq_getattr(cola, &attr);
	printf("Cantidad maxima de mensajes admitidos: %d\n", (int) attr.mq_maxmsg);
    printf("Tamanio maximo del mensaje: %d\n", (int) attr.mq_msgsize);
    printf("Cantidad actual de mensajes: %d\n", (int) attr.mq_curmsgs);
	mq_close(cola);
}

void escuchaActivaCola(const char *colaNombre){
	/** Abrir y revisar la cola ( ͡° ͜ʖ ͡°) */
	mqd_t cola = mq_open(colaNombre, O_RDONLY);
	if (cola == (mqd_t)-1){
		perror("mq_open");
		exit(EXIT_FAILURE);
	}
	msg_t msg;
	while(1){
		if(mq_receive(cola, (char*) &msg, sizeof(msg_t), NULL) < 0){
			perror("mq_receive");
			exit(EXIT_FAILURE);
		}
		printf("%s: %s\n", msg.sender, msg.text);
   	}

}
