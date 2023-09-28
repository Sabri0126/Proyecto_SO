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
	//creacion de semaforos
	sem_init(&sem_a,0,1);
	sem_init(&sem_b,0,0);
	sem_init(&sem_c,0,0);
	sem_init(&sem_d,0,0);
	
	//creacion de hilos
	pthread_create(&threadA,NULL,&a,NULL);
	pthread_create(&threadB,NULL,&b,NULL);
	pthread_create(&threadC,NULL,&c,NULL);
	pthread_create(&threadD,NULL,&d,NULL);

	pthread_join(threadA,NULL);
	pthread_join(threadB,NULL);
	pthread_join(threadC,NULL);
	pthread_join(threadD,NULL);
}