#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define RESERVADO 1
#define NO_RESERVADO 0
#define CANT_ALUMNOS 25
#define CANT_HORAS_MAX 12

pthread_t alumnos[CANT_ALUMNOS];
pthread_mutex_t mutex,rMutex;

int horas[CANT_HORAS_MAX],
consultores = 0; //se utiliza para controlar la rafaga de consultas

void *alumno( void *arg);

int main(){
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&rMutex,NULL);

    //inicializo la tabla de reservas
    for (int i = 0; i < CANT_HORAS_MAX; ++i) {
        horas[i] = NO_RESERVADO;
    }

    //creacion de hilos
    for (int i = 0; i < CANT_ALUMNOS; ++i) {
        if(pthread_create(&alumnos[i], NULL, &alumno, (void *) i) != 0){
            printf("Error al crear el hilo.\n");
            return 1;
        }
    }

    //finalizacion de hilos
    for (int i = 0; i < CANT_ALUMNOS; ++i) {
        if(pthread_join(alumnos[i],NULL) != 0){
           printf("Error al hacer join.\n");
            return 2; 
        }
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&rMutex);
    return 0;
}

void *alumno(void *arg){
    int nroAlumno = (int) arg;
    int horaReservada = -1, opcion,horaSeleccionada;

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
                pthread_mutex_lock(&mutex);
                if(horas[horaSeleccionada] == RESERVADO){
                    printf("El alumno %d intenta reservar a las %d pero ya se encuentra reservada. \n",nroAlumno,horaSeleccionada+9);
                }else{
                    horas[horaSeleccionada] = RESERVADO;
                    horaReservada=horaSeleccionada;
                    printf("El alumno %d reservo la computadora a las %d horas.\n",nroAlumno,horaSeleccionada+9);
                }
                pthread_mutex_unlock(&mutex);
            }
        }else if((opcion >= 50) && (opcion <= 74)){
            //Cancela reserva
            if(horaReservada < 0){ //ya reservo una hora
                printf("El alumno %d no puede cancelar una reserva que no tiene. \n",nroAlumno);
            }else{
                pthread_mutex_lock(&mutex);
                horas[horaReservada] = NO_RESERVADO;
                printf("El alumno %d cancelo la reserva de las %d horas.\n",nroAlumno,horaReservada+9);
                horaReservada = -1;
                pthread_mutex_unlock(&mutex);
            }
        }else{
            //Consultar reserva
            horaSeleccionada = rand() % CANT_HORAS_MAX;
            pthread_mutex_lock(&rMutex);
            consultores++;
            if (consultores == 1){ //si soy el primero que consulta
                pthread_mutex_lock(&mutex); //bloqueo la tabla de reservas
            }
            pthread_mutex_unlock(&rMutex);

            //consulto tabla
            if(horas[horaSeleccionada] == RESERVADO){
                printf("El alumno %d consultó si la computadora esta reservada a las %d horas y resultó que lo estaba.\n", nroAlumno, horaSeleccionada+9);
            }else{
                printf("El alumno %d consultó si la computadora esta reservada a las %d horas y resultó que no lo estaba.\n", nroAlumno, horaSeleccionada+9);
            }

            pthread_mutex_lock(&rMutex);
            consultores--;
            if (consultores == 0){ //si soy el ultimo que consulta
                pthread_mutex_unlock(&mutex); //desbloqueo la tabla de reservas
            }
            pthread_mutex_unlock(&rMutex);
        }
        sleep(2);
    }

    pthread_exit(NULL);
}