#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int pipeA[2];
int pipeB[2];
int pipeC[2];

void a(){
  int entero;
	//pipes de lectura
	close(pipeA[1]);
	
	//pipes de escritura
	close(pipeB[0]);

	//otros
	close(pipeC[0]);
	close(pipeC[1]);

	while (1)
	{
		read(pipeA[0], &entero, sizeof(int));
		printf("A");
		fflush(stdout);
		write(pipeB[1], &entero, sizeof(int));
	}
}

void b(){
  int entero;
	//pipes de lectura
	close(pipeB[1]);
	
	//pipes de escritura
	close(pipeA[0]);
	close(pipeC[0]);

	while (1)
	{
		read(pipeB[0], &entero, sizeof(int));
		printf("B");
		fflush(stdout);
		write(pipeA[1], &entero, sizeof(int));
		
		read(pipeB[0], &entero, sizeof(int));
		printf("B");
		fflush(stdout);
		write(pipeC[1], &entero, sizeof(int));
	}
}

void c(){
  int entero;
	//pipes de lectura
	close(pipeC[1]);
	
	//pipes de escritura
	close(pipeA[0]);

	//otros
	close(pipeB[0]);
	close(pipeB[1]);

	while (1)
	{
		read(pipeC[0], &entero, sizeof(int));
		printf("C");
		fflush(stdout);
		write(pipeA[1], &entero, sizeof(int));
	}
}

int main(){
    int pid;
	int entero;
	
    if(pipe(pipeA) == -1){return -1;}
    if(pipe(pipeB) == -1){return -1;}
    if(pipe(pipeC) == -1){return -1;}
    
    entero = 1; 
    //Inicializamos los pipes correspondientes
    write(pipeA[1], &entero, sizeof(int));
    
    //Empezaremos a crear los hijos del padre
    pid = fork(); 
    if(pid < 0){
        printf("Error al crear un proceso ");
    }else if(pid==0){
        //Ejecutamos a
        a();       
    }
		
	pid = fork(); 
    if(pid < 0){
        printf("Error al crear un proceso ");
    }else if(pid==0){
        //Ejecutamos b
        b();       
    }
		
	pid = fork(); 
    if(pid < 0){
        printf("Error al crear un proceso ");
    } else if(pid==0){
        //Ejecutamos c
        c();       
    }

	for(int i = 0; i<3; i++){
        wait(NULL);
    }
}