#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/msg.h>

#define maxCapacidadFilas 15
#define maxCapacidadMesa 30
#define cantTipoClientes 3
const char Clientes[cantTipoClientes] = {'E','U','P'}; //Arreglo con los tipos de clientes.
int clientesMesa, clientesUsuarios, clientesPoliticos, clientesEmpresas;

void mesa_de_entrada(int msg_idMesa);
void filas_especificas(int msg_idMesa, int msg_idEmp, int msg_idPol, int msg_idUsu);
void empleados(int i, int msg_idEmp, int msg_idPol, int msg_idUsu);

struct message {
	long type;
	char cliente;
};
const int size = sizeof(struct message) - sizeof(long);

int main(){
    int msg_idMesa, msg_idEmp, msg_idPol, msg_idUsu;

    // Creamos las colas de mensajes
    msg_idMesa = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msg_idMesa == -1) {
        printf("Error al crear la cola de mensajes de la mesa de entrada.\n");
        fflush(stdout);
        return 1;
    }

    msg_idEmp = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msg_idEmp == -1) {
        printf("Error al crear la cola de mensajes de la fila de empresas.\n");
        fflush(stdout);
        return 1;
    }

    msg_idPol = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msg_idPol == -1) {
        printf("Error al crear la cola de mensajes de la fila de politicos.\n");
        fflush(stdout);
        return 1;
    }

    msg_idUsu = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
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
    msgctl(msg_idMesa,IPC_RMID,NULL); //Elimina las colas
    msgctl(msg_idEmp,IPC_RMID,NULL);
    msgctl(msg_idPol,IPC_RMID,NULL);
    msgctl(msg_idUsu,IPC_RMID,NULL);
    return 0;
}

void mesa_de_entrada(int msg_idMesa) {
    srand(time(NULL));
    char tipo_cliente = ' ';
    int num_random = 0;
    clientesMesa = 0;
    while(1) {
        num_random = rand() % 3; // Generamos un numero random entre 0 y 2
        tipo_cliente = Clientes[num_random]; // Usamos el numero random para simular un nuevo cliente
        printf("Llego un cliente de tipo %c al banco.\n",tipo_cliente);
        struct message m = {1, tipo_cliente}; // Mensaje que vamos a enviar
        struct msqid_ds info; // Variable que usaremos para comprobar la cantidad de mensajes en la cola
        if (msgctl(msg_idMesa, IPC_STAT, &info) == -1) {
            printf("Error al obtener información de la cola de mensajes.\n");
            exit(1);
        }
        clientesMesa = info.msg_qnum; // Numero de clientes en la mesa de entrada
        if(clientesMesa < maxCapacidadMesa) { // Verificamos que la cantidad de mensajes en la cola sea menor a 30 
            if(msgsnd(msg_idMesa, &m, size, 0) == -1){ // Ingresa un cliente a la mesa de entrada
                printf("Error al escribir el mensaje %c\n",tipo_cliente);
                exit(1);
            }
            clientesMesa++;
            printf("Ingresa un cliente de tipo %c a la mesa de entrada.\nClientes en Mesa de Entrada: %d.\n",tipo_cliente,clientesMesa);
            fflush(stdout);
        } else { // Si la mesa de entrada esta llena
            printf("Cliente se retira porque la mesa de entrada está llena.\n");
            fflush(stdout);
        }
        sleep(1);
    }
    exit(0);
}

void filas_especificas(int msg_idMesa, int msg_idEmp, int msg_idPol, int msg_idUsu) {
    sleep(5);
    clientesPoliticos = 0;
    clientesEmpresas = 0;
    clientesUsuarios = 0;
    while(1){
        struct message m; // Variable que va a guardar el mensaje recibido
        struct msqid_ds info; // Variable que usaremos para comprobar la cantidad de clientes en las filas
        if(msgrcv(msg_idMesa, &m, size, 1, 0) == -1){ // Recibe un cliente de la mesa de entrada
			printf("Error al leer el mensaje %c\n",m.cliente);
			fflush(stdout);
            exit(1);
		}
        switch (m.cliente) {
            case 'E': {
                if (msgctl(msg_idEmp, IPC_STAT, &info) == -1) {
                    printf("Error al obtener información de la cola de mensajes.\n");
                    exit(1);
                }
                clientesEmpresas = info.msg_qnum; // Numero de clientes esperando en la cola de empresas
                if(clientesEmpresas < maxCapacidadFilas) { // Comprueba se la fila esta llena
                    if(msgsnd(msg_idEmp, &m, size, 0) == -1){ // Despacha al cliente a la fila de empresas
                        printf("Error al escribir el mensaje %c\n",m.cliente);
                        exit(1);
                    } else {
                        clientesMesa--;
                        clientesEmpresas++;
                        printf("Ingresa un cliente a la fila de empresas.\nClientes en la fila de Empresas: %d.\n",clientesEmpresas);
                        fflush(stdout);
                    }
                } else { // Si la fila de empresas esta llena
                    printf("Fila de empresas llena, el cliente espera en la entrada.\n");
                    fflush(stdout);
                    if(msgsnd(msg_idMesa, &m, size, 0) == -1){ // El cliente vuelve a la mesa de entrada
                        printf("Error al escribir el mensaje %c\n",m.cliente);
                        exit(1);
                    }
                }
                break;
            }
            case 'U': {
                if (msgctl(msg_idUsu, IPC_STAT, &info) == -1) {
                    printf("Error al obtener información de la cola de mensajes.\n");
                    exit(1);
                }
                clientesUsuarios = info.msg_qnum; // Numero de clientes esperando en la cola de usuarios
                if(clientesUsuarios < maxCapacidadFilas) { // Comprueba si la fila esta llena
                    if(msgsnd(msg_idUsu, &m, size, 0) == -1){ // Despacha al cliente a la fila de Usuarios
                        printf("Error al escribir el mensaje %c\n",m.cliente);
                        exit(1);
                    } else {
                        clientesUsuarios++;
                        clientesMesa--;
                        printf("Ingresa un cliente a la fila de usuarios.\nClientes en la fila de Usuarios: %d.\n",clientesUsuarios);
                        fflush(stdout);
                    }
                } else { // Si la fila de usuarios esta llena
                    printf("Fila de usuarios llena, el cliente espera en la entrada.\n");
                    fflush(stdout);
                    if(msgsnd(msg_idMesa, &m, size, 0) == -1){ // El cliente vuelve a la mesa de entrada
                        printf("Error al escribir el mensaje %c\n",m.cliente);
                        exit(1);
                    }
                }
                break;
            }
            case 'P': {
                if (msgctl(msg_idPol, IPC_STAT, &info) == -1) {
                    printf("Error al obtener información de la cola de mensajes.\n");
                    exit(1);
                }
                clientesPoliticos = info.msg_qnum; // Numero de clientes esperando en la cola de politicos
                if(clientesPoliticos < maxCapacidadFilas) {
                    if(msgsnd(msg_idPol, &m, size, 0) == -1){
                        printf("Error al escribir el mensaje %s\n",m.cliente);
                        exit(1);
                    } else {
                        clientesPoliticos++;
                        clientesMesa--;
                        printf("Ingresa un cliente a la fila de politicos.\nClientes en la fila de Politicos: %d.\n",clientesPoliticos);
                        fflush(stdout);
                    }
                } else { // Si la fila de politicos esta llena
                    printf("Fila de politicos llena, el cliente espera en la entrada.\n");
                    fflush(stdout);
                    if(msgsnd(msg_idMesa, &m, size, 0) == -1){ // El cliente vuelve a la mesa de entrada
                        printf("Error al escribir el mensaje %c\n",m.cliente);
                        exit(1);
                    }
                }
                break;
            }
        }
        sleep(1);
    }
    exit(0);
}

/* Empleado 0 y 1 atienden Empresas
 * Empleado 2 atiende Usuarios
 * Los 3 empleados atienden Politicos*/
void empleados(int i, int msg_idEmp, int msg_idPol, int msg_idUsu) {
    sleep(10); // Delay grande para permitir que las filas se vayan llenando antes de comenzar a atender
    struct message m;
    while (1) {
        if(msgrcv(msg_idPol, &m, size, 1, IPC_NOWAIT) != -1){ // Los tres empleados atienden politicos primero
            printf("Un politico es atendido y se retira.\n");
            sleep(6);
			fflush(stdout);
		} else { // No hay politicos para atender. Cuando esto ocurre los empleados atienden las otras filas
            if(i == 0 || i == 1) { // Empleados 0 y 1: Atienden empresas
                if(msgrcv(msg_idEmp, &m, size, 1, IPC_NOWAIT) != -1){
			        printf("Una empresa es atendida y se retira.\n");
                    fflush(stdout);
                    sleep(7);
		        }
            } else { // Empleado 2: Atiende usuarios
                if(msgrcv(msg_idUsu, &m, size, 1, IPC_NOWAIT) != -1){
			        printf("Un usuario es atendido y se retira.\n");
                    fflush(stdout);
                    sleep(5);
                }       
            }
        }
    }
    exit(0);  
}