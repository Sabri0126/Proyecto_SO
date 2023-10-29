#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>


int main(int argc, char ** argv){
    DIR *dir;
    struct dirent *dirent; //Tiene información sobre el archivo actual que se está obteniendo
    char* dirName;
    if(argc != 2){
        printf("Error: La entrada ingresada no es valida, escriba 'help' para mas ayuda sobre el uso del comando. \n");
        exit(EXIT_FAILURE);
    }else if ((dir = opendir (argv[1])) != NULL){
        //Mientras hayan elementos dentro del directorio por leer
        while ((dirent = readdir(dir)) != NULL){
            dirName = dirent->d_name;
            if(strcmp(dirName,".")!=0 && strcmp(dirName, "..")!=0)
                printf ("%s\n", dirName);
        } 
    }else{
        printf("Error: no se pudo listar el contenido del directorio\n");
        closedir(dir);
        exit(EXIT_FAILURE);
    }
    closedir(dir);
    exit(EXIT_SUCCESS);
}

