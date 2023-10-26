#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char ** argv){
  if (argv[1] == NULL) {
        printf("Comando 'quit': Finaliza la ejecucion de la MiniShell. \n\n");
        printf("Comando 'help': Muestra una pequeña ayuda con respecto a los comandos válidos para la MiniShell. \n\n");
        printf("Comando 'mkdir [nombreDirectorio]': crea un nuevo directorio.\n");
        printf("Comando 'ldir [nombreDirectorio]': lista el contenido de un directorio.\n");
        printf("Comando 'rmdir [nombreDirectorio]': elimina el directorio seleccionado y todo su contenido.\n");
        printf("Comando 'mkfile [nombreArchivo]': crea un nuevo archivo.\n");
        printf("Comando 'lfile [nombreArchivo]': muestra el contenido de un archivo.\n");
        printf("Comando 'chmod [nombreArchivo] [permiso]': cambia los permisos otorgados al propietario de un archivo.\n\n");
        printf("Los permisos disponibles son:\n");
        printf("  # 400: Lectura\n");
        printf("  # 200: Escritura\n");
        printf("  # 100: Ejecución\n");
        printf("  # 600: Lectura y escritura\n");
        printf("  # 500: Lectura y ejecución\n");
        printf("  # 300: Escritura y ejecución\n");
        printf("  # 700: Lectura, escritura y ejecución\n\n");
        exit(EXIT_SUCCESS);        
 } else{
        printf("Error: este comando no admite parámetros adicionales.\n");
        exit(EXIT_FAILURE);
 }
    
}
