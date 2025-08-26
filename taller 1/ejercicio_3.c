#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <syscall.h>
#include <errno.h>

void handlerSIGURGHijo(int signum){
    printf("ya va \n");
}

void suicidio(int signum){
    exit(0);
}   

void hijo(){
    signal(SIGURG, handlerSIGURGHijo); // Ignoramos la señal
    signal(SIGINT, suicidio);
    while(1){}
}

int main(int argc, char const *argv[]){

    pid_t id_hijo = fork();
    if (id_hijo == 0)
        hijo();

    for (size_t i = 0; i < 5; i++)
    {
        sleep(1);
        printf("sup \n");
        kill(id_hijo, SIGURG);
    }

    // Ejecutamos el programa pasado por argumento
    char *args[argc]; // Array para los argumentos
    for (size_t i = 1; i < argc; i++) {
        args[i - 1] = (char *)argv[i];
    }
    args[argc - 1] = NULL;
    
    kill(id_hijo, SIGINT); 
    wait(NULL);    
    execvp(args[0], args);

    return 0;
}
