#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// Uso procesos y pipes
#define maxCapacidadFilas 15
#define maxCapacidadMesa 30
#define cantTipoClientes 3
const char Clientes[cantTipoClientes] = {'e','u','p'};

//Creacion de pipes
int mesa_entrada[2], cola_empresas[2], cola_usuarios[2], cola_politicos[2];

int main(){
    
    // Ingreso de los clientes
    int pid = fork();
    if(pid == 0){
        mesa_de_entrada();
    }

    // Creamos los empleados que atienden empresas
    for(int i = 0; i < 2; i++){
        int pid = fork();
        if(pid == 0) {
            empleadoEmp(); 
        }
    }

    // Creamos el empleado que atiende usuarios
    int pid = fork();
    if(pid == 0){
        empleadoUsu();
    }

    return 0;
}


void mesa_de_entrada() {
    char colaEntrada[maxCapacidadMesa];
    int colaEmpresa = 0;
    int colaUsuarios = 0;
    int colaPoliticos = 0;
    char tipo_cliente = ' ';
    int num_random = 0;
    int i = 0; //Variable que incrementa cuando ingresa un cliente a la mesa de entrada
    while (1) {
        num_random = rand() % 3; 
        tipo_cliente = Clientes[num_random];
        if(i < maxCapacidadMesa) {
            colaEntrada[i] = tipo_cliente;
            i++;
            printf("Ingresa un cliente a la mesa de entrada");
            switch (tipo_cliente) {
                case 'e': {
                    if(colaEmpresa < maxCapacidadFilas) {

                        write(cola_empresas[1],&tipo_cliente,1);
                    }
                    
                    break;
                }
                case 'u': {
                    write(cola_usuarios[1],&tipo_cliente,1);
                    break;
                }
                case 'p': {
                    write(cola_politicos[1], &tipo_cliente, 1);
                    break;
                }
            }
        } else {
            printf("Cliente se retira porque la mesa de entrada está llena.\n");
        }
    }
    exit(0);
}

void empleadoEmp() {
    char cliente = ' ':
    while(1) {
        if(read(cola_politicos[0], &cliente, 1) == -1) {
            printf("No hay ningun politico en la fila");
        } 
    }
}

void empleadoUsu() {

}