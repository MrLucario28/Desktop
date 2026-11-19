

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N        5          /*numero de filósofos y tenedores */
#define RONDAS   3          /* veces que filósofo piensa y come */
#define SHARED   0          /* semforos solo entre hilos del mismo proceso */

#define IZQ(i)   (((i) + N - 1) % N)  /*vecino izquierdo*/
#define DER(i)   (((i) + 1) % N)     /*vecino derecho */


static sem_t tenedor[N];          /* un semáforo (candado) por tenedor, inicia en 1 */

/* Verificación: nunca deben comer dos vecinos a la vez */
static int   comiendo[N];
static sem_t mutex_check;

static void dormir_aleatorio(unsigned int *semilla, int max_ms)
{
    usleep((rand_r(semilla) % max_ms) * 1000);
}

static void adquirir_tenedores(int i)
{
    if (i == N - 1) {                 
        sem_wait(&tenedor[DER(i)]);   
        sem_wait(&tenedor[i]);        
    } else {                          
        sem_wait(&tenedor[i]);       
        sem_wait(&tenedor[DER(i)]);   
    }
}

static void liberar_tenedores(int i)
{
    sem_post(&tenedor[i]);        
    sem_post(&tenedor[DER(i)]);       
}

static void comer(int i, unsigned int *semilla)
{
    sem_wait(&mutex_check);
    if (comiendo[IZQ(i)] || comiendo[DER(i)])
        printf("*** ERROR: el filósofo %d come junto a un vecino que también come ***\n", i);
    comiendo[i] = 1;
    sem_post(&mutex_check);

    printf("Filósofo %d está COMIENDO\n", i);
    dormir_aleatorio(semilla, 300);

    sem_wait(&mutex_check);
    comiendo[i] = 0;
    sem_post(&mutex_check);
}

/*  Hilo filosofo*/
static void *filosofo(void *arg)
{
    int i = *(int *)arg;
    unsigned int semilla = 1234u + (unsigned int)i;

    for (int r = 0; r < RONDAS; r++) {
        printf("    Filósofo %d está PENSANDO\n", i);
        dormir_aleatorio(&semilla, 300);         

        adquirir_tenedores(i);                 
        comer(i, &semilla);                       
        liberar_tenedores(i);                     
    }

    printf("Filósofo %d terminó sus %d rondas.\n", i, RONDAS);
    return NULL;
}

int main(void)
{
    pthread_t hilos[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        sem_init(&tenedor[i], SHARED, 1);        
        comiendo[i] = 0;
    }
    sem_init(&mutex_check, SHARED, 1);

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        if (pthread_create(&hilos[i], NULL, filosofo, &ids[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < N; i++)
        pthread_join(hilos[i], NULL);

    for (int i = 0; i < N; i++)
        sem_destroy(&tenedor[i]);
    sem_destroy(&mutex_check);

    printf("\nFin: todos los filósofos terminaron sin bloqueo mutuo.\n");
    return EXIT_SUCCESS;
}