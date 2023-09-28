#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define maxCapacidadFilas 15
#define maxCapacidadMesa 30
#define cantTipoClientes 3
const char Clientes[cantTipoClientes] = {'E','U','P'}; //Arreglo con los tipos de clientes.

void mesa_de_entrada(int msg_idMesa);
void filas_especificas(int msg_idMesa, int msg_idEmp, int msg_idPol, int msg_idUsu);
void empleados(int i, int msg_idEmp, int msg_idPol, int msg_idUsu);

struct message {
	long type;
	char cliente;
};
const int size = sizeof(struct message) - sizeof(long);


int main(){
    key_t keyMesa, keyEmp, keyPol, keyUsu;
    int msg_idMesa, msg_idEmp, msg_idPol, msg_idUsu;

    // Generar una clave única
    keyMesa = ftok("mesa", 'A');
    keyEmp = ftok("empresas", 'B');
    keyPol = ftok("politicos", 'C');
    keyUsu = ftok("usuarios", 'D');

    // Creamos las colas de mensajes
    msg_idMesa = msgget(keyMesa, 0666 | IPC_CREAT);
    if (msg_idMesa == -1) {
        printf("Error al crear la cola de mensajes de la mesa de entrada.\n");
        fflush(stdout);
        return 1;
    }

    msg_idEmp = msgget(keyEmp, 0666 | IPC_CREAT);
    if (msg_idEmp == -1) {
        printf("Error al crear la cola de mensajes de la fila de empresas.\n");
        fflush(stdout);
        return 1;
    }

    msg_idPol = msgget(keyPol, 0666 | IPC_CREAT);
    if (msg_idPol == -1) {
        printf("Error al crear la cola de mensajes de la fila de politicos.\n");
        fflush(stdout);
        return 1;
    }

    msg_idUsu = msgget(keyUsu, 0666 | IPC_CREAT);
    if (msg_idUsu == -1) {
        printf("Error al crear la cola de mensajes de la fila de usuarios.\n");
        fflush(stdout);
        return 1;
    }

    // Ingreso de los clientes
    int pid0 = fork();
    if(pid0 < 0){
        printf("Error al crear el proceso.\n");
        fflush(stdout);
        return 2;
    }
    if(pid0 == 0){
        mesa_de_entrada(msg_idMesa);
    }

    // Acceso a las filas especificas
    int pid1 = fork();
    if(pid1 < 0){
        printf("Error al crear el proceso.\n");
        fflush(stdout);
        return 2;
    }
    if(pid1 == 0){
        filas_especificas(msg_idMesa, msg_idEmp, msg_idPol, msg_idUsu);
    }

    // Creamos los empleados
    for(int i = 0; i < 3; i++){
        int pid2 = fork();
        if(pid2 < 0){
            printf("Error al crear el proceso.\n");
            fflush(stdout);
            return 2;
        }
        if(pid2 == 0) {
            empleados(i, msg_idEmp, msg_idPol, msg_idUsu); 
        }
    }

    wait(NULL); // Espera a los procesos hijos
    return 0;
}

void mesa_de_entrada(int msg_idMesa) {
    char tipo_cliente = ' ';
    int num_random = 0;
    while(1) {
        num_random = rand() % 3; // Generamos un numero random entre 0 y 2
        tipo_cliente = Clientes[num_random]; // Usamos el numero random para simular un nuevo cliente
        struct message m = {1, tipo_cliente}; // Mensaje que vamos a enviar
        struct msqid_ds info; // Variable que usaremos para comprobar la cantidad de mensajes en la cola
        if (msgctl(msg_idMesa, IPC_STAT, &info) == -1) {
            printf("Error al obtener información de la cola de mensajes.\n");
            exit(1);
        }
        if(info.msg_qnum < maxCapacidadMesa) { // Verificamos que la cantidad de mensajes en la cola sea menor a 30 
            if(msgsnd(msg_idMesa, &m, size, 0) == -1){
                printf("Error al escribir el mensaje %c\n",tipo_cliente);
                exit(1);
            }
            printf("Ingresa un cliente de tipo %c a la mesa de entrada.\n",tipo_cliente);
            fflush(stdout);
        } else { // Si la mesa de entrada esta llena
            printf("Cliente se retira porque la mesa de entrada está llena.\n");
            fflush(stdout);
        }
        printf("Hay %s clientes en la mesa de entrada.\n",info.msg_qnum);
        fflush(stdout);
        sleep(5);
    }
    exit(0);
}

void filas_especificas(int msg_idMesa, int msg_idEmp, int msg_idPol, int msg_idUsu) {
    sleep(1);
    while(1){
        struct message m; // Variable que va a guardar el mensaje recibido
        struct msqid_ds info; // Variable que usaremos para comprobar la cantidad de clientes en las filas
        if(msgrcv(msg_idMesa, &m, size, 1, 0) == -1){
			printf("Error al leer el mensaje %s\n",m.cliente);
			fflush(stdout);
            exit(1);
		}
        switch (m.cliente) {
            case 'E': {
                if (msgctl(msg_idEmp, IPC_STAT, &info) == -1) {
                    printf("Error al obtener información de la cola de mensajes.\n");
                    exit(1);
                }
                if(info.msg_qnum < maxCapacidadFilas) {
                    if(msgsnd(msg_idEmp, &m, size, 0) == -1){
                        printf("Error al escribir el mensaje %s\n",m.cliente);
                        exit(1);
                    }
                    printf("Ingresa un cliente a la fila de empresas.\n");
                    fflush(stdout);
                } else { // Si la fila de empresas esta llena
                    printf("Fila de empresas llena, el cliente espera en la entrada.\n");
                    fflush(stdout);
                }
                break;
            }
            case 'U': {
                if (msgctl(msg_idUsu, IPC_STAT, &info) == -1) {
                    printf("Error al obtener información de la cola de mensajes.\n");
                    exit(1);
                }
                if(info.msg_qnum < maxCapacidadFilas) {
                    if(msgsnd(msg_idUsu, &m, size, 0) == -1){
                        printf("Error al escribir el mensaje %s\n",m.cliente);
                        exit(1);
                    }
                    printf("Ingresa un cliente a la fila de usuarios.\n");
                    fflush(stdout);
                } else { // Si la fila de usuarios esta llena
                    printf("Fila de usuarios llena, el cliente espera en la entrada.\n");
                    fflush(stdout);
                }
                break;
            }
            case 'P': {
                if (msgctl(msg_idPol, IPC_STAT, &info) == -1) {
                    printf("Error al obtener información de la cola de mensajes.\n");
                    exit(1);
                }
                if(info.msg_qnum < maxCapacidadFilas) {
                    if(msgsnd(msg_idPol, &m, size, 0) == -1){
                        printf("Error al escribir el mensaje %s\n",m.cliente);
                        exit(1);
                    }
                    printf("Ingresa un cliente a la fila de empresas.\n");
                    fflush(stdout);
                } else { // Si la fila de politicos esta llena
                    printf("Fila de politicos llena, el cliente espera en la entrada.\n");
                    fflush(stdout);
                }
                break;
            }
        }
        sleep(5);
    }
    exit(0);
}

/* Empleado 0 y 1 atienden Empresas
 * Empleado 2 atiende Usuarios
 * Los 3 empleados atienden Politicos*/
void empleados(int i, int msg_idEmp, int msg_idPol, int msg_idUsu) {
    sleep(5);
    struct message m;
    while (1) {
        // Como los politicos son prioridad, revisamos siempre primero su fila
        if(msgrcv(msg_idPol, &m, size, 1, 0) == -1){
			printf("No hay ningun cliente de prioridad esperando ser atendido.\n");
			fflush(stdout);
            continue;
		} else {
            printf("Un politico es atendido y se retira.\n");
			fflush(stdout);
            // El tiempo que tarda en atenderlo, es un numero random entre 1 y 6
            sleep(rand() % 6 + 1);
        }

        if(i == 0 || i == 1) { // Atienden empresas
            if(msgrcv(msg_idEmp, &m, size, 1, 0) == -1){
			    printf("Error al leer el mensaje %s\n",m.cliente);
			    fflush(stdout);
                exit(1);
		    } else {
                printf("Una empresa es atendida y se retira.\n");
			    fflush(stdout);
                sleep(rand() % 6 + 1);
            }
        } else { // Atiende usuarios
             if(msgrcv(msg_idUsu, &m, size, 1, 0) == -1){
			    printf("Error al leer el mensaje %s\n",m.cliente);
			    fflush(stdout);
                exit(1);
		    } else {
                printf("Un usuario es atendido y se retira.\n");
			    fflush(stdout);
                sleep(rand() % 6 + 1);
            }       
        }
    }
    exit(0);  
}