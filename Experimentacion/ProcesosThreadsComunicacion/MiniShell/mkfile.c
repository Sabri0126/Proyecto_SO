#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv){
    FILE *file;
    //Corroboramos los parametros
    if (argc != 2){
        printf("Error: La entrada ingresada no es valida, escriba 'help' para mas ayuda sobre el uso del comando.\n");
        exit(EXIT_FAILURE);
    } else {

        //Verifiquemos que el archivo existe
        if(access(argv[1], F_OK) == 0){
            printf("Error: el archivo ya existe dentro del directorio\n");
            exit(EXIT_FAILURE);
        } else {
            file = fopen(argv[1],"w");
            fclose(file);
            printf("El archivo se ha creado con exito\n");
            exit(EXIT_SUCCESS);
        }
    }
}
