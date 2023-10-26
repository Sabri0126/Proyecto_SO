#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <wait.h>
#include <sys/shm.h>

#define RESERVADO 1
#define NO_RESERVADO 0
#define CANT_ALUMNOS 25
#define CANT_HORAS_MAX 12
struct computadora{
    int turno[CANT_HORAS_MAX]; /* dato del segmento */
};

#define KEY ((key_t) (1243)) /* número de llave */
#define SEGSIZE sizeof (struct computadora) /* longitud del segmento */

sem_t mutex;

void alumno(int nroAlumno);

int main(){
    int id,pid;
    struct computadora *aula;
    sem_init(&mutex,1,1);
    id = shmget(KEY, SEGSIZE, IPC_CREAT | 0666);
    if (id <0){
        printf(" FALLO EL shmget \n"); exit(EXIT_FAILURE);
    }
    aula = (struct computadora*) shmat(id,0,0);
    if (aula <= (struct computadora *) (0)){
        printf(" Error en el shmat \n"); exit(EXIT_FAILURE);
    }

    //inicializo la tabla de reservas
    for (int i = 0; i < CANT_HORAS_MAX; ++i) {
        aula->turno[i] = NO_RESERVADO;
    }

    for (int i =0; i<CANT_ALUMNOS; i++) {
        pid = fork();				// se hace fork()

        if (pid==0) {				// si el proceso se crea bien, terminamos el ciclo for
            alumno(i);
            break;
        }
        else if (pid==-1) {			// si hay error, se aborta la operación
            perror("ERROR al hacer fork()");
            exit(EXIT_FAILURE);
            break;
        }
    }

    for (int i=0; i<CANT_ALUMNOS; i++) {		// esperamos a que todos los hijos terminen
       wait(NULL);
    }

    sem_destroy(&mutex);
    return 0;
}

void alumno(int nroAlumno){
    int horaReservada = -1, opcion,horaSeleccionada,id;
    struct computadora *aula;

    //vinculo el segmento de memoria
    id = shmget(KEY, SEGSIZE, IPC_CREAT | 0666);
    if (id <0){
        printf(" FALLO EL shmget \n"); exit(EXIT_FAILURE);
    }
    aula = (struct computadora*) shmat(id,0,0);
    if (aula <= (struct computadora *) (0)){
        printf(" Error en el shmat \n"); exit(EXIT_FAILURE);
    }

    //inicializo el generador pseudoaleatorio
    srand((unsigned int) time(NULL));

    while (1){
        opcion = rand() % 100;
        if((opcion >= 0) && (opcion <= 49)){
            //Reserva de aula
            if(horaReservada > 0){ //ya reservo una hora
                printf("el alumno %d ya tiene una hora reservada. \n",nroAlumno);
            }else{
                horaSeleccionada = rand() % CANT_HORAS_MAX;
                sem_wait(&mutex);
                if(aula->turno[horaSeleccionada] == RESERVADO){
                    printf("El alumno %d intenta reservar a las %d pero ya se encuentra reservada. \n",nroAlumno,horaSeleccionada+9);
                }else{
                    aula->turno[horaSeleccionada] = RESERVADO;
                    horaReservada=horaSeleccionada;
                    printf("El alumno %d reservo la computadora a las %d horas.\n",nroAlumno,horaSeleccionada+9);
                }
                sem_post(&mutex);
            }
        }else if((opcion >= 50) && (opcion <= 74)){
            //Cancela reserva
            if(horaReservada < 0){ //ya reservo una hora
                printf("El alumno %d no puede cancelar una reserva que no tiene. \n",nroAlumno);
            }else{
                sem_wait(&mutex);
                aula->turno[horaReservada] = NO_RESERVADO;
                printf("El alumno %d cancelo la reserva de las %d horas.\n",nroAlumno,horaReservada+9);
                horaReservada = -1;
                sem_post(&mutex);
            }
        }else{
            //Consultar reserva
            horaSeleccionada = rand() % CANT_HORAS_MAX;
            if(aula->turno[horaSeleccionada] == RESERVADO){
                printf("El alumno %d consultó si la computadora esta reservada a las %d horas y resultó que lo estaba.\n", nroAlumno, horaSeleccionada+9);
            }else{
                printf("El alumno %d consultó si la computadora esta reservada a las %d horas y resultó que no lo estaba.\n", nroAlumno, horaSeleccionada+9);
            }
        }
        sleep(2);
    }

    exit(EXIT_SUCCESS);
}