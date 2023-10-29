#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


int main(int argc, char ** argv){
    //Debemos verificar que la cantidad de argumentos sea válida
    if(argc != 2){
        printf("Error: La entrada ingresada no es valida, escriba 'help' para mas ayuda sobre el uso del comando.\n");
        exit(EXIT_FAILURE);
    }else if(mkdir(argv[1],S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0){
            printf("Directorio creado.\n");
            exit(EXIT_SUCCESS);
    }
    else{
        printf("Error: el directorio %s no pudo ser creado.\n",argv[1]);
        exit(EXIT_FAILURE);
    }
}
