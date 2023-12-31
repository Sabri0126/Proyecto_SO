#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define maxCapacidadFilas 15
#define maxCapacidadMesa 30

sem_t sem_mesa; // Semaforo para la mesa de entrada
sem_t sem_filaUsu_vacio, sem_filaUsu_lleno; // Semaforo para la fila de usuarios comunes
sem_t sem_filaEmp_vacio, sem_filaEmp_lleno; // Semaforo para la fila de empresas
sem_t sem_filaPol_vacio, sem_filaPol_lleno; // Semaforo para la fila de politicos
sem_t sem_empleadoPol, sem_empleadoUsu, sem_empleadoEmp; // Semaforo para atender un tipo de cliente
sem_t sem_usuario_atendido, sem_empresa_atendida, sem_politico_atendido; // Semaforo para finalizar la atencion de un cliente

pthread_t hilosClientes[100], hilosEmpleados[3];

void *politicos();
void *empresas();
void *usuarios();
void *empleadoUsuario();
void *empleadosEmpresas();


int main(){
    // Inicialización de semáforos
    // Mesa de entrada
    sem_init(&sem_mesa, 1, maxCapacidadMesa);
    // Filas especificas
    sem_init(&sem_filaUsu_vacio, 1, maxCapacidadFilas);
    sem_init(&sem_filaUsu_lleno, 1, 0);
    sem_init(&sem_filaEmp_vacio, 1, maxCapacidadFilas);
    sem_init(&sem_filaEmp_lleno, 1, 0);
    sem_init(&sem_filaPol_vacio, 1, maxCapacidadFilas);
    sem_init(&sem_filaPol_lleno, 1, 0);
    // Empleados y atenciones
    sem_init(&sem_empleadoPol, 1, 0);
    sem_init(&sem_empleadoUsu, 1, 0);
    sem_init(&sem_empleadoEmp, 1, 0);
    sem_init(&sem_usuario_atendido, 1, 0);
    sem_init(&sem_empresa_atendida, 1, 0);
    sem_init(&sem_politico_atendido, 1, 0);

    // Creamos los empleados de empresas
    for (int i = 0; i < 2; i++) {
        if(pthread_create(&hilosEmpleados[i], NULL, empleadosEmpresas, (void *) i) != 0){
            printf("Error al crear el hilo.\n");
            return 1;
        }
    }

    // Creamos el empleado de usuarios
    if(pthread_create(&hilosEmpleados[2], NULL, empleadoUsuario, NULL) != 0){
        printf("Error al crear el hilo.\n");
        return 1;
    }
    
    // Generamos tipos de clientes aleatorios
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        int tipo_cliente = 0;
        tipo_cliente = rand() % 3 + 1; // Genera un numero random entre 1 y 3
        switch(tipo_cliente){
	        case 1: {
                pthread_create(&hilosClientes[i], NULL, usuarios, (void *) i);
		        break;
	        }
	        case 2: {
                pthread_create(&hilosClientes[i], NULL, empresas, (void *) i);
		        break;
            }
            case 3: {
                pthread_create(&hilosClientes[i], NULL, politicos, (void *) i);
		        break;                
            }
	    }
    }

    // Join clientes
    for (int i = 0; i < 100; i++) {
        if(pthread_join(hilosClientes[i], NULL) != 0){
            printf("Error al hacer join.\n");
            return 2;
        }
    }

    // Join empleados
    for (int i = 0; i < 3; i++) {
        if(pthread_join(hilosEmpleados[i], NULL) != 0){
            printf("Error al hacer join.\n");
            return 2;
        }
    }

    // Destruimos los semaforos
    sem_destroy(&sem_mesa);
    sem_destroy(&sem_filaUsu_vacio);
    sem_destroy(&sem_filaUsu_lleno);
    sem_destroy(&sem_filaEmp_vacio);
    sem_destroy(&sem_filaEmp_lleno);
    sem_destroy(&sem_filaPol_vacio);
    sem_destroy(&sem_filaPol_lleno);
    sem_destroy(&sem_empleadoPol);
    sem_destroy(&sem_empleadoUsu);
    sem_destroy(&sem_empleadoEmp);
    sem_destroy(&sem_usuario_atendido);
    sem_destroy(&sem_empresa_atendida);
    sem_destroy(&sem_politico_atendido);

    return 0;
}


void *politicos(void *arg){
    int id_cliente = (int) arg;
    printf("Politico %d llega al banco y quiere entrar.\n",id_cliente);
    if(sem_trywait(&sem_mesa) == 0){
        // Hay lugar en la mesa de entrada
        printf("El politico %d ingresa a la mesa de entrada.\n",id_cliente);
        sem_wait(&sem_filaPol_vacio); // Intenta entrar en su fila, sino espera
        printf("El politico %d ingresa a la fila de atencion de politicos.\n",id_cliente);
        sem_post(&sem_mesa); // Libera su lugar en la mesa
        sem_post(&sem_filaPol_lleno); // Aumenta la cantidad de lugares ocupados
        sem_wait(&sem_empleadoPol); // Espera a ser atendido
        sem_post(&sem_politico_atendido); // Indica que ha finalizado su atencion
        sem_post(&sem_filaPol_vacio); // Libera su lugar en la fila
        printf("El politico %d fue atendido y se retira.\n",id_cliente);
    } else {
        // No hay lugar en la mesa de entrada
        printf("El politico %d se retira porque la mesa de entrada está llena.\n",id_cliente);
    }
}

void *empresas(void *arg){
    int id_cliente = (int) arg;
    printf("Empresa %d llega al banco y quiere entrar.\n",id_cliente);
    if(sem_trywait(&sem_mesa) == 0){
        // Hay lugar en la mesa de entrada
        printf("La empresa %d ingresa a la mesa de entrada.\n",id_cliente);
        sem_wait(&sem_filaEmp_vacio); // Intenta entrar en su fila, sino espera
        printf("La empresa %d ingresa a la fila de atencion de empresas.\n",id_cliente);
        sem_post(&sem_mesa); // Libera su lugar en la mesa
        sem_post(&sem_filaEmp_lleno); // Aumenta la cantidad de lugares ocupados
        sem_wait(&sem_empleadoEmp); // Espera a ser atendido
        sem_post(&sem_empresa_atendida); // Indica que ha finalizado su atencion
        sem_post(&sem_filaEmp_vacio); // Libera su lugar en la fila
        printf("La empresa %d fue atendida y se retira.\n",id_cliente);
    } else {
        // No hay lugar en la mesa de entrada
        printf("La empresa %d se retira porque la mesa de entrada está llena.\n",id_cliente);
    }
}

void *usuarios(void *arg){
    int id_cliente = (int) arg;
    printf("Usuario %d llega al banco y quiere entrar.\n",id_cliente);
    if(sem_trywait(&sem_mesa) == 0){
        // Hay lugar en la mesa de entrada
        printf("El usuario %d ingresa a la mesa de entrada.\n",id_cliente);
        sem_wait(&sem_filaUsu_vacio); // Intenta entrar en su fila, sino espera
        printf("El usuario %d ingresa a la fila de atencion de usuarios.\n",id_cliente);
        sem_post(&sem_mesa); // Libera su lugar en la mesa
        sem_post(&sem_filaUsu_lleno); // Aumenta la cantidad de lugares ocupados
        sem_wait(&sem_empleadoUsu); // Espera a ser atendido
        sem_post(&sem_usuario_atendido); // Indica que ha finalizado su atencion
        sem_post(&sem_filaUsu_vacio); // Libera su lugar en la fila
        printf("El usuario %d fue atendido y se retira.\n",id_cliente);
    } else {
        // No hay lugar en la mesa de entrada
        printf("El usuario %d se retira porque la mesa de entrada está llena.\n",id_cliente);
    }
}

void *empleadosEmpresas(void *arg){
    int id_empleado = (int) arg;
    while(1){
        if(sem_trywait(&sem_filaPol_lleno) == 0){
            // Hay politicos esperando
            sem_post(&sem_empleadoPol); // Manda una señal para comenzar la atencion
            printf("Un empleado esta atendiendo un politico. (id_empleado: %d)\n",id_empleado);
            sem_wait(&sem_politico_atendido); // Espera a que termine de atenderse
            printf("El empleado termino de atender un politico. (id_empleado: %d)\n",id_empleado);
        } else {
            // No hay politicos esperando
            sem_wait(&sem_filaEmp_lleno); // Espera a que haya una empresa en la fila
            sem_post(&sem_empleadoEmp); // Manda una señal para comenzar la atencion
            printf("Un empleado esta atendiendo una empresa. (id_empleado: %d)\n",id_empleado);
            sem_post(&sem_empresa_atendida);
            printf("El empleado termino de atender una empresa. (id_empleado: %d)\n",id_empleado);
        }
    }
}

void *empleadoUsuario(){
    int id_empleado = 2;
    while(1){
        if(sem_trywait(&sem_filaPol_lleno) == 0){
            // Hay politicos esperando
            sem_post(&sem_empleadoPol); // Manda una señal para comenzar la atencion
            printf("Un empleado esta atendiendo un politico. (id_empleado: %d)\n",id_empleado);
            sem_wait(&sem_politico_atendido); // Espera a que termine de atenderse
            printf("El empleado termino de atender un politico. (id_empleado: %d)\n",id_empleado);
        } else {
            // No hay politicos esperando
            sem_wait(&sem_filaUsu_lleno); // Espera a que haya un usuario en la fila
            sem_post(&sem_empleadoUsu); // Manda una señal para comenzar la atencion
            printf("Un empleado esta atendiendo un usuario. (id_empleado: %d)\n",id_empleado);
            sem_post(&sem_usuario_atendido); // Espera a que termine de atenderse
            printf("El empleado termino de atender un usuario. (id_empleado: %d)\n",id_empleado);
        }        
    }
}