#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv) {
    FILE *file;
    int c;
    int isEnd = 1;
       
    //Corroboramos la cantidad de parámetros válida   
    if (argc != 2){
       printf("Error: La entrada ingresada no es valida, escriba 'help' para mas ayuda sobre el uso del comando.\n");
       exit(EXIT_FAILURE);
    }else {
       //Solo queremos listar su contenido por lo que los abrimos en modo lectura
       file = fopen(argv[1],"r");
       if(file == NULL) {
           printf("Error al abrir el archivo");
          fclose(file);
          exit(EXIT_FAILURE);
       }else{ 
              while(isEnd){
                     c = fgetc(file);
                     //Si estamos al final del archivo, ya habremos terminado de listar su contenido
                     if(feof(file))
                         isEnd = 0;
                     else
                            printf("%c", c);
              }
       }
       fclose(file);
       exit(EXIT_SUCCESS);
    }
}
