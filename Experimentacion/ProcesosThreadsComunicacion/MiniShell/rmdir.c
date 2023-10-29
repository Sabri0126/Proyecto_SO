#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv){
    //Debemos verificar que la cantidad de argumentos sea válida
    if (argc != 2){
        printf("Error: La entrada ingresada no es valida, escriba 'help' para mas ayuda sobre el uso del comando.\n");
    } else {
        if(rmdir(argv[1]) == 0){
            printf("Se elimino el directorio de forma exitosa.\n");
            exit(EXIT_SUCCESS);
        } else {
            printf("Error: no se pudo eliminar el directorio correctamente.\n");
            exit(EXIT_FAILURE);
        }
    }
}
