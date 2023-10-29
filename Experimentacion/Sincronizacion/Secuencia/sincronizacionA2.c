#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
	Secuencia
	ABABCABCD ABABCABCD
*/

pthread_t threadA,threadB,threadC,threadD;
sem_t sem_a,sem_b,sem_c,sem_d;

void *a(){
	while(1){
		sem_wait(&sem_a);
		printf("A");
		fflush(stdout);
		sem_post(&sem_b);
	}
}

void *b(){
	while (1){
		sem_wait(&sem_b);
		printf("B");
		fflush(stdout);
		sem_post(&sem_a);
		for(int i = 0;i<2;i++){
			sem_wait(&sem_b);
			printf("B");
			fflush(stdout);
			sem_post(&sem_c);
		}
	}
}

void *c(){
	while(1){
		sem_wait(&sem_c);
		printf("C");
		fflush(stdout);
		sem_post(&sem_a);
		sem_wait(&sem_c);
		printf("C");
		fflush(stdout);
		sem_post(&sem_d);
	}
}

void *d(){
	while(1){
		sem_wait(&sem_d);
		printf("D");
		fflush(stdout);
		sem_post(&sem_a);
	}
}

int main(){
	//Creacion de semaforos
	sem_init(&sem_a,0,1);
	sem_init(&sem_b,0,0);
	sem_init(&sem_c,0,0);
	sem_init(&sem_d,0,0);
	
	//Creacion de hilos
	if(pthread_create(&threadA,NULL,&a,NULL) != 0){
		printf("Error al crear el hilo.\n");
        return 1;
	}

	if(pthread_create(&threadB,NULL,&b,NULL) != 0){
		printf("Error al crear el hilo.\n");
        return 1;
	}

	if(pthread_create(&threadC,NULL,&c,NULL) != 0){
		printf("Error al crear el hilo.\n");
        return 1;
	}

	if(pthread_create(&threadD,NULL,&d,NULL) != 0){
		printf("Error al crear el hilo.\n");
        return 1;
	}

	if(pthread_join(threadA,NULL) != 0){
		printf("Error al hacer join.\n");
        return 2;
	}

	if(pthread_join(threadB,NULL) != 0){
		printf("Error al hacer join.\n");
        return 2;
	}

	if(pthread_join(threadC,NULL) != 0){
		printf("Error al hacer join.\n");
        return 2;
	}

	if(pthread_join(threadD,NULL) != 0){
		printf("Error al hacer join.\n");
        return 2;
	}

	// Destruimos los semaforos
    sem_destroy(&sem_a);
    sem_destroy(&sem_b);
    sem_destroy(&sem_c);
    sem_destroy(&sem_d);
}