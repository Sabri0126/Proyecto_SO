#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DELIM " \t\r\n\a"

void executeCmd(char** args);
void welcome();

int main () {
    int c, i, pos;
    int canContinue = 1;
    char *cmd = malloc(sizeof(char *) * 64);
    char **args = (char **) malloc(64 * sizeof(char **));

    //Aparece la presentación de la minishell con los comandos que pueden ser ejecutados
    welcome();

    while (canContinue) {
        printf("minishell@~$ ");
        i = 0;
        pos = 0;
        //Damos lugar a la lectura de la línea de hasta encontrar un salto, es decir, que el usuario haya realizado "enter"
        while (1) {
            c = getchar();
            if (c == '\n') {
                cmd[i] = '\0';
                break;
            } else {
                cmd[i] = c;
                i++;
            }
        }
        //Separaremos los argumentos de la cadena leída recientemente
        char *argument = (char *) strtok(cmd, DELIM);
        while (argument != NULL) {
            args[pos] = argument;
            pos++;
            argument = (char *) strtok(NULL, DELIM);
        }
        args[pos] = NULL;
        if (strcmp(args[0], "quit") == 0)
            canContinue = !canContinue;
        else
            //Si el usuario no hay finalizado la MiniShell, entonces intentamos ejecutar
            executeCmd(args);
    }

    free(cmd);
    free(args);
    return EXIT_SUCCESS;
}

void welcome(){
    printf("-MINISHELL-\n");
    printf("En caso de necesitar ayuda, por favor, ingrese el comando 'help'.\n");
    printf("Si desea salir de la MiniShell ingrese el comando 'quit'.\n");
    printf("Desarrollado por: Sabrina Barrionuevo y Nahuel Diaz.\n\n\n");
}

void executeCmd(char** args){
    int pid;
    int status;
    pid = vfork();
    if (pid == 0){
        if(execv(args[0],args) == -1)
            perror("Error al cargar programa\n");
        else
            printf("Comando inválido\n");
    }else if (pid < 0) {
        perror("Error durante la creación del proceso\n");
    }else {
        wait(&status);
    }
}