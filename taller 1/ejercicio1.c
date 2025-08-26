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

int n;
int numero_maldito;

pid_t child_pids[10];

int generate_random_number(){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL) ^ getpid());
    return (rand() % n);
}

void hijoSigtermHandler(int signum){
    int numero_azar = generate_random_number();
    printf("Hijo %d: He recibido la señal %d. Mi número es %d. Mi número maldito es %d\n", getpid(), signum, numero_azar, numero_maldito);
    if(numero_azar == numero_maldito){
        printf("Hijo %d: He muerto porque mi número es el maldito\n", getpid());
        exit(0);
    } else {
        printf("Hijo %d: Sigo vivo\n", getpid());
    }
}

void hijo(){
    signal(SIGTERM, hijoSigtermHandler);
    while(1){}
}

void receiveSigchild(int signum){
    pid_t hijo_muerto;
    printf("Padre: He recibido la señal %d\n", signum);
    hijo_muerto = waitpid(-1, NULL, WNOHANG);
    for (size_t i = 0; i < n; i++)
    {
        if (child_pids[i] == hijo_muerto)
        {
            child_pids[i] = -1; // Marcamos el hijo como muerto
            printf("Padre: El hijo %d ha muerto\n", hijo_muerto);
        }
    }
}

int main(int argc, char const *argv[]){
    
    n = atoi(argv[1]);
	int rondas = atoi(argv[2]);
	numero_maldito = atoi(argv[3]);

    int pid_actual;

    // Creamos todos los hijos
    for (size_t i = 0; i < n; i++)
    {
        printf("Creando hijo %d\n", i);
        int pid_actual = fork();
        if (pid_actual != 0)
        {
            child_pids[i] = pid_actual;
        }
        else {
            hijo();
            break;
        }
    }

    // Definimos el manejador de la señal SIGCHLD para el padre
    signal(SIGCHLD, receiveSigchild);

    // Arranca el juego
    for (size_t i = 0; i < rondas; i++){
        printf("Ronda %d\n", i);

        // Enviamos SIGTERM a todos los hijos
        for (size_t j = 0; j < n; j++)
        {
            if (child_pids[j] != -1) 
            {
                kill(child_pids[j], SIGTERM); 
                sleep(1);
            }
            
        }
    }

    // Informamos de los hijos que sobrevivieron
    for (size_t j = 0; j < n; j++)
    {
        if (child_pids[j] != -1) // Si el child pid no es
        {
            printf("Sobrevivió el hijo %d\n", child_pids[j]);        
            kill(child_pids[j], SIGKILL); // Los matamos
        }
    }

    return 0;
}
