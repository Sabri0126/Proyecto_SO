#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
    char * fileName;
    char * perm;
    int permNum,status;

    //Corroboramos que la cantidad de parámetros sea exactamente 3
    if (argc != 3){
        printf("Error: Cantidad de parametros invalida\n"); 
        exit(EXIT_FAILURE);
    }else{
        fileName = (char *)malloc(1+strlen(argv[1]));
        strcpy(fileName,argv[1]);

        //Corroboramos que el archivo al que queremos cambiarle los permisos efectivamente exista
        if(access(fileName, F_OK) != 0){
            printf("El archivo no existe\n");
            free(fileName);
            exit(EXIT_FAILURE);
        }else {
            perm = (char *)malloc(1+strlen(argv[2]));
            strcpy(perm, argv[2]);

            //Cambiamos el permiso
            permNum = strtol(perm,0,8);
            status = chmod(fileName,permNum);
            if(status != 0){
                printf("No se pudo modificar los permisos del archivo %s, a %s\n",argv[1],argv[2]);
                free(perm);
                free(fileName);
                exit(EXIT_FAILURE);
            } else {
                printf("Permiso actualizado correctamente.\n");
            }
            free(perm);
            free(fileName);
            exit(EXIT_SUCCESS);
        }
    }
}
