#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define maxCapacidadFilas 15
#define maxCapacidadMesa 30
#define cantTipoClientes 3
const char Clientes[cantTipoClientes] = {'E','U','P'}; //Arreglo con los tipos de clientes.
char Mesa[maxCapacidadMesa], Emp[maxCapacidadFilas], Pol[maxCapacidadFilas], Usu[maxCapacidadFilas];
int clientesMesa = 0, clientesUsuarios = 0, clientesPoliticos = 0, clientesEmpresas = 0;
int posClienteP = 0, posClienteE = 0, posClienteU = 0, posCliente = 0; // Indice para obtener los clientes de la mesa en orden FIFO
int im = 0, ip = 0, ie = 0, iu = 0; // Indices de cada fila de espera

sem_t sem_mesa, sem_colaEmp, sem_colaPol, sem_colaUsu, mutex;
pthread_t hilo_mesa, hilo_filas, hilo_empleados[3];

void *mesa_de_entrada(){
    srand(time(NULL));
    char tipo_cliente = ' ';
    int num_random = 0;
    while (1) {
        num_random = rand() % 3; // Generamos un número aleatorio entre 0 y 2
        tipo_cliente = Clientes[num_random]; // Usamos el número aleatorio para simular un nuevo cliente      
        printf("Llego un cliente de tipo %c al banco.\n",tipo_cliente);
        if (sem_trywait(&sem_mesa) == 0) { // Prueba si hay lugar en la mesa de entrada
            sem_wait(&mutex); // Inicia la seccion critica
            Mesa[im] = tipo_cliente;
            clientesMesa++;
            if(im == maxCapacidadMesa) im = 0;
            else im++;
            printf("Ingresa un cliente de tipo %c a la mesa de entrada.\n", tipo_cliente);
            printf("Clientes en Mesa de Entrada: %d.\n", clientesMesa);
            fflush(stdout);
            sem_post(&mutex); // Fin seccion critica
        } else {
            printf("Cliente se retira porque la mesa de entrada está llena.\n");
        }
        sleep(rand() % 2 + 1);
    }    
    exit(0);
}

void *filas_especificas(){
    sleep(5);
    srand(time(NULL));
    while(1){
        sem_wait(&mutex); // Inicio seccion critica
        char cliente = ' ';
        if(clientesMesa > 0){ // Verifico que hayan ingresado clientes a la mesa de entrada
            cliente = Mesa[posCliente]; //Extraemos un clientes de la mesa
            Mesa[posCliente] = ' ';
            sem_post(&sem_mesa);
            if(posCliente == maxCapacidadMesa) posCliente = 0; // Si llego al final, que vuelva
            else posCliente++; // Sino que aumente una unidad
            switch (cliente){
                case 'E':{
                    if(sem_trywait(&sem_colaEmp) == 0){
                        Emp[ie] = cliente;
                        clientesEmpresas++;
                        clientesMesa--;
                        if(ie == maxCapacidadFilas) ie = 0; // Si llego al final, que vuelva
                        else ie++; // Sino que aumente una unidad
                        printf("Ingresa un cliente a la fila de empresas.\nClientes en la fila de Empresas: %d.\n",clientesEmpresas);
                        fflush(stdout);
                    } else {
                        printf("Fila de empresas llena, el cliente espera en la entrada.\n");
                        fflush(stdout);
                        Mesa[im] = cliente; // Vuelve a la mesa de entrada
                        sem_wait(&sem_mesa);
                        if(im == maxCapacidadMesa) im = 0;
                        else im++;
                    }
                    break;
                }
                case 'U': {
                    if(sem_trywait(&sem_colaUsu) == 0){
                        Usu[iu] = cliente;
                        clientesUsuarios++;
                        clientesMesa--;
                        if(iu == maxCapacidadFilas) iu = 0; // Si llego al final, que vuelva
                        else iu++; // Sino que aumente una unidad                       
                        printf("Ingresa un cliente a la fila de usuarios.\nClientes en la fila de Usuarios: %d.\n",clientesUsuarios);
                        fflush(stdout);
                    } else {
                        printf("Fila de usuarios llena, el cliente espera en la entrada.\n");
                        fflush(stdout);
                        Mesa[im] = cliente; // Vuelve a la mesa de entrada (al final de la fila)
                        sem_wait(&sem_mesa);
                        if(im == maxCapacidadMesa) im = 0;
                        else im++;
                    }
                    break;
                }
                case 'P': {
                    if(sem_trywait(&sem_colaPol) == 0){
                        Pol[ip] = cliente;
                        clientesPoliticos++;
                        clientesMesa--;
                        if(ip == maxCapacidadFilas) ip = 0; // Si llego al final, que vuelva
                        else ip++; // Sino que aumente una unidad
                        printf("Ingresa un cliente a la fila de politicos.\nClientes en la fila de Politicos: %d.\n",clientesPoliticos);
                        fflush(stdout);
                    } else {
                        printf("Fila de politicos llena, el cliente espera en la entrada.\n");
                        fflush(stdout);
                        Mesa[im] = cliente; // Vuelve a la mesa de entrada
                        sem_wait(&sem_mesa);
                        if(im == maxCapacidadMesa) im = 0;
                        else im++;
                    }
                    break;
                }
            }
        }
        sem_post(&mutex); // Fin seccion critica
        sleep(rand() % 2 + 1);
    }
    exit(0);
}

void *empleados(void *arg){
    sleep(20);
    srand(time(NULL));
    int id_empleado = (int) arg;
    while(1){
        sem_wait(&mutex);
        char cliente = ' ';
        if(clientesPoliticos > 0){
            cliente = Pol[posClienteP];
            Pol[posClienteP] = ' ';
            if(posClienteP == maxCapacidadFilas) posClienteP = 0;
            else posClienteP++;
            printf("Un politico es atendido y se retira.\n");
            fflush(stdout);
            clientesPoliticos--;
            sem_post(&sem_colaPol); // Libera el lugar
        } else { // No hay politicos para atender. Cuando esto ocurre los empleados atienden las otras filas
            if((id_empleado == 0 || id_empleado == 1)){ // Empleados 0 y 1: Atienden empresas
                if(clientesEmpresas > 0){ // Verificamos que haya empresas en la fila
                    cliente = Emp[posClienteE];
                    Emp[posClienteE] = ' ';
                    if(posClienteE == maxCapacidadFilas) posClienteE = 0;
                    else posClienteE++;
                    printf("Una empresa es atendida y se retira.\n");
                    fflush(stdout);
                    clientesEmpresas--;
                    sem_post(&sem_colaEmp); // Libera el lugar
                }
            } else { // Empleado 2: Atiende usuarios
                if(clientesUsuarios > 0){ // Verificamos que haya usuarios en la fila
                    cliente = Usu[posClienteU];
                    Usu[posClienteU] = ' ';
                    if(posClienteU == maxCapacidadFilas) posClienteU = 0;
                    else posClienteU++;
                    printf("Un usuario es atendido y se retira.\n");
                    fflush(stdout);
                    clientesUsuarios--;
                    sem_post(&sem_colaUsu); // Libera el lugar
                }
            }
        }
        sem_post(&mutex);
        sleep(rand() % 5 + 3);
    }
    exit(0);
}

int main(){
    // Inicialización de semáforos
    sem_init(&sem_mesa, 0, maxCapacidadMesa);
    sem_init(&sem_colaEmp, 0, maxCapacidadFilas);
    sem_init(&sem_colaPol, 0, maxCapacidadFilas);
    sem_init(&sem_colaUsu, 0, maxCapacidadFilas);
    sem_init(&mutex, 0, 1);

    // Creacion de hilos
    if(pthread_create(&hilo_mesa, NULL, &mesa_de_entrada, NULL) != 0){
        printf("Error al crear el hilo.\n");
        return 1;
    };


    if(pthread_create(&hilo_filas, NULL, &filas_especificas, NULL) != 0){
        printf("Error al crear el hilo.\n");
        return 1;
    };

 
    for (int i = 0; i < 3; i++) {
        if(pthread_create(&hilo_empleados[i], NULL, &empleados, (void *) i) != 0){
            printf("Error al crear el hilo.\n");
            return 1;
        }
    }
    
    if(pthread_join(hilo_mesa,NULL) != 0){
        printf("Error al hacer join.\n");
        return 2;
    };

    if(pthread_join(hilo_filas, NULL) != 0){
        printf("Error al hacer join.\n");
        return 2;
    };

    for (int i = 0; i < 3; i++){
        if(pthread_join(hilo_empleados[i], NULL) != 0){
            printf("Error al hacer join.\n");
            return 2;
        };
    }

    // Destruimos los semaforos
    sem_destroy(&sem_mesa);
    sem_destroy(&sem_colaEmp);
    sem_destroy(&sem_colaPol);
    sem_destroy(&sem_colaUsu);
    sem_destroy(&mutex);
    return 0;
}