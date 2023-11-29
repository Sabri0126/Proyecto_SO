#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

// Capacidades maximas
#define maxCapacidadFilas 15
#define maxCapacidadMesa 30
//Mesa de entrada
#define mesa_entrada 1
// Filas especificas
#define fila_usuarios_vacio 2
#define fila_usuarios_lleno 3
#define fila_politicos_vacio 4
#define fila_politicos_lleno 5
#define fila_empresas_vacio 6
#define fila_empresas_lleno 7
// Empleados y atenciones
#define usuario_atendido 8
#define empleado_usuario 9
#define politico_atendido 10
#define empleado_politico 11
#define empresa_atendida 12
#define empleado_empresa 13

struct message {
		long type;
		char msg;
};
const int size = sizeof(struct message) - sizeof(long);

void usuarios(int i, int msg_id);
void empresas(int i, int msg_id);
void politicos(int i, int msg_id);
void empleadosEmpresas(int i, int msg_id);
void empleadoUsuario(int msg_id);

int main(){
	key_t key;
	int msg_id;   

    // Generar clave
    key = ftok("/tmp", 'A');

    // Crear la cola
    msg_id = msgget(key, 0666 | IPC_CREAT);
    if (msg_id == -1) {
        printf("Error al crear la cola de mensajes.\n");
        return 1;
    }

    // Inicializamos la cola de mensajes con los lugares disponibles
    struct message m;
    // Mesa de entrada
	for(int i = 0; i < maxCapacidadMesa; i++){
		m.type = mesa_entrada;
		msgsnd(msg_id, &m, size,0);
	}

	// Usuarios
	for(int i = 0; i < maxCapacidadFilas; i++){
		m.type = fila_usuarios_vacio;
		msgsnd(msg_id, &m, size,0);
	}
	
	// Politicos
	for(int i = 0 ; i < maxCapacidadFilas ; i++){
		m.type = fila_politicos_vacio;
		msgsnd(msg_id, &m, size,0);
	}
	
	// Empresas
	for(int i = 0 ; i < maxCapacidadFilas ; i++){
		m.type = fila_empresas_vacio;
		msgsnd(msg_id, &m, size,0);
	}

    // Creamos los empleados de empresas
    for(int i = 0; i < 2; i++){
        int pid = fork();
        if(pid == 0){
            empleadosEmpresas(i,msg_id);
        }
    }

    // Creamos el empleado de usuarios
    int pid = fork();
    if(pid == 0){
        empleadoUsuario(msg_id);
    }

    // Generamos tipos de clientes aleatorios 
    for(int i = 0; i < 100; i++){
        int pid = fork();
        if(pid == 0){
            srand(time(NULL)*getpid());
            int tipo_cliente = 0;
            tipo_cliente = rand() % 3 + 1;
            switch(tipo_cliente){
                case 1: {
                    usuarios(i,msg_id);
                    break;
                }
                case 2: {
                    empresas(i,msg_id);
                    break;
                }
                case 3: {
                    politicos(i,msg_id);
                    break;                
                }
            }
        }
    }

    for(int i = 0 ; i < 103; i++){
		wait(NULL);
	}

    // Eliminar la cola
    msgctl(msg_id, IPC_RMID, NULL);

    return 0;
}

void usuarios(int i, int msg_id){
    int id_cliente = i;
    struct message m;
    printf("Usuario %d llega al banco y quiere entrar.\n",id_cliente);
	if(msgrcv(msg_id, &m, size, mesa_entrada, IPC_NOWAIT) != -1){
		// Hay lugar en la mesa de entrada
		printf("El usuario %d ingresa a la mesa de entrada.\n",id_cliente); 
		msgrcv(msg_id, &m, size, fila_usuarios_vacio, 0); // Intenta entrar en su fila, sino espera
		printf("El usuario %d ingresa a la fila de atencion de usuarios.\n",id_cliente);
	
		m.type = mesa_entrada; 
		msgsnd(msg_id, &m, size,0); // Libera su lugar en la mesa
		
		m.type = fila_usuarios_lleno; 
		msgsnd(msg_id, &m, size,0); // Aumenta la cantidad de lugares ocupados
		
		msgrcv(msg_id, &m, size, empleado_usuario, 0); // Espera a ser atendido
        printf("El usuario %d es atendido\n",id_cliente);
		
		m.type = usuario_atendido;
		msgsnd(msg_id, &m, size,0); // Indica que ha finalizado su atencion

		m.type = fila_usuarios_vacio;
		msgsnd(msg_id, &m, size,0); // Libera su lugar en la fila
		printf("El usuario %d fue atendido y se retira.\n",id_cliente);
	} else {
		// No hay lugar en la mesa de entrada
		printf("El usuario %d se retira porque la mesa de entrada está llena.\n",id_cliente);
	}
    exit(0);
}

void empresas(int i, int msg_id){
    int id_cliente = i;
    struct message m;
    printf("Empresa %d llega al banco y quiere entrar.\n",id_cliente);
	if(msgrcv(msg_id, &m, size, mesa_entrada, IPC_NOWAIT) != -1){
		// Hay lugar en la mesa de entrada
		printf("La empresa %d ingresa a la mesa de entrada.\n",id_cliente); 
		msgrcv(msg_id, &m, size, fila_empresas_vacio, 0); // Intenta entrar en su fila, sino espera
		printf("La empresa %d ingresa a la fila de atencion de empresas.\n",id_cliente);
	
		m.type = mesa_entrada; 
		msgsnd(msg_id, &m, size, 0); // Libera su lugar en la mesa
		
		m.type = fila_empresas_lleno; 
		msgsnd(msg_id, &m, size, 0); // Aumenta la cantidad de lugares ocupados
		
		msgrcv(msg_id, &m, size, empleado_empresa, 0); // Espera a ser atendido
        printf("La empresa %d es atendido\n",id_cliente);
		
		m.type = empresa_atendida;
		msgsnd(msg_id, &m, size, 0); // Indica que ha finalizado su atencion

		m.type = fila_empresas_vacio;
		msgsnd(msg_id, &m, size, 0); // Libera su lugar en la fila
		printf("La empresa %d fue atendido y se retira.\n",id_cliente);
	} else {
		// No hay lugar en la mesa de entrada
		printf("La empresa %d se retira porque la mesa de entrada está llena.\n",id_cliente);
	}
    exit(0);
}

void politicos(int i, int msg_id){
    int id_cliente = i;
    struct message m;
    printf("Politico %d llega al banco y quiere entrar.\n",id_cliente);
	if(msgrcv(msg_id, &m, size, mesa_entrada, IPC_NOWAIT) != -1){
		// Hay lugar en la mesa de entrada
		printf("El politico %d ingresa a la mesa de entrada.\n",id_cliente); 
		msgrcv(msg_id, &m, size, fila_politicos_vacio, 0); // Intenta entrar en su fila, sino espera
		printf("El politico %d ingresa a la fila de atencion de politicos.\n",id_cliente);
	
		m.type = mesa_entrada; 
		msgsnd(msg_id, &m, size, 0); // Libera su lugar en la mesa
		
		m.type = fila_politicos_lleno; 
		msgsnd(msg_id, &m, size, 0); // Aumenta la cantidad de lugares ocupados
		
		msgrcv(msg_id, &m, size, empleado_politico, 0); // Espera a ser atendido
        printf("El politico %d es atendido.\n",id_cliente);
		
		m.type = politico_atendido;
		msgsnd(msg_id, &m, size, 0); // Indica que ha finalizado su atencion

		m.type = fila_politicos_vacio;
		msgsnd(msg_id, &m, size, 0); // Libera su lugar en la fila
		printf("El politico %d fue atendido y se retira.\n",id_cliente);
	} else {
		// No hay lugar en la mesa de entrada
		printf("El politico %d se retira porque la mesa de entrada está llena.\n",id_cliente);
	}
    exit(0);
}

void empleadosEmpresas(int i, int msg_id){
    int id_empleado = i;
    struct message m;
	while(1){
		if(msgrcv(msg_id, &m, size, fila_politicos_lleno, IPC_NOWAIT) != -1){
			// Hay politicos esperando
			printf("Un empleado esta atendiendo un politico. (id_empleado: %d)\n",id_empleado);
			m.type = empleado_politico;
			msgsnd(msg_id, &m, size, 0); // Manda una señal para comenzar la atencion
			msgrcv(msg_id, &m, size, politico_atendido, 0); // Espera a que termine de atenderse
			printf("El empleado termino de atender un politico. (id_empleado: %d)\n",id_empleado);
		}else{
			// No hay politicos esperando
			msgrcv(msg_id, &m, size, fila_empresas_lleno, 0); // Espera a que haya una empresa en la fila
			printf("Un empleado esta atendiendo una empresa. (id_empleado: %d)\n",id_empleado);
			m.type = empleado_empresa;
			msgsnd(msg_id, &m, size, 0); // Manda una señal para comenzar la atencion
			msgrcv(msg_id, &m, size, empresa_atendida, 0); // Espera a que termine de atenderse
			printf("El empleado termino de atender una empresa. (id_empleado: %d)\n",id_empleado);
		}
	}
    exit(0);
}

void empleadoUsuario(int msg_id){
    int id_empleado = 2;
    struct message m;
	while(1){
		if(msgrcv(msg_id, &m, size, fila_politicos_lleno, IPC_NOWAIT) != -1){
			// Hay politicos esperando
			printf("Un empleado esta atendiendo un politico. (id_empleado: %d)\n",id_empleado);
			m.type = empleado_politico;
			msgsnd(msg_id, &m, size, 0); // Manda una señal para comenzar la atencion
			msgrcv(msg_id, &m, size, politico_atendido, 0); // Espera a que termine de atenderse
			printf("El empleado termino de atender un politico. (id_empleado: %d)\n",id_empleado);
		}else{
			// No hay politicos esperando
			msgrcv(msg_id, &m, size, fila_usuarios_lleno, 0); // Espera a que haya un usuario en la fila
			printf("Un empleado esta atendiendo un usuario. (id_empleado: %d)\n",id_empleado);
			m.type = empleado_usuario;
			msgsnd(msg_id, &m, size, 0); // Manda una señal para comenzar la atencion
			msgrcv(msg_id, &m, size, usuario_atendido, 0); // Espera a que termine de atenderse
			printf("El empleado termino de atender un usuario. (id_empleado: %d)\n",id_empleado);
		}
	}
    exit(0);
}
