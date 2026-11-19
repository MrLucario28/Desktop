

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N                    5     
#define NUM_PRODUCTORES      2    
#define NUM_CONSUMIDORES     2     
#define ITEMS_POR_PRODUCTOR  10
#define TOTAL_ITEMS          (NUM_PRODUCTORES * ITEMS_POR_PRODUCTOR)
#define ITEMS_POR_CONSUMIDOR (TOTAL_ITEMS / NUM_CONSUMIDORES)

#define SHARED 0   

_Static_assert(TOTAL_ITEMS % NUM_CONSUMIDORES == 0,
               "TOTAL_ITEMS debe ser divisible entre NUM_CONSUMIDORES");

static int buf[N];
static int front = 0;   /* índice del primer mensaje a consumir        */
static int rear  = 0;   /* índice del primer casillero vacío           */

static sem_t empty;     /* cuenta espacios vacíos                      */
static sem_t full;      /* cuenta espacios llenos                      */
static sem_t mutexP;    /* exclusión mutua entre productores (rear)    */
static sem_t mutexC;    /* exclusión mutua entre consumidores (front)  */

/* ------- Productor[i] ------- */
static void *productor(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < ITEMS_POR_PRODUCTOR; i++) {
        int data = id * 100 + i;              /* produce data            */
        usleep((rand() % 300) * 1000);        /* simula el tiempo de producción */

        sem_wait(&empty);                     /* P(empty): espera espacio */
        sem_wait(&mutexP);                    /* P(mutexP)                */

        buf[rear] = data;                     /* sección crítica de productores */
        printf("Productor %d depositó %3d en la posición %d\n", id, data, rear);
        rear = (rear + 1) % N;

        sem_post(&mutexP);                    /* V(mutexP)                */
        sem_post(&full);                      /* V(full): hay un dato más */
    }

    printf("Productor %d terminó.\n", id);
    return NULL;
}

/* -------- Consumidor[i] ------- */
static void *consumidor(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < ITEMS_POR_CONSUMIDOR; i++) {
        sem_wait(&full);                      /* P(full): espera un dato  */
        sem_wait(&mutexC);                    /* P(mutexC)                */

        int pos = front;                      /* sección crítica de consumidores */
        int result = buf[front];
        front = (front + 1) % N;

        sem_post(&mutexC);                    /* V(mutexC)                */
        sem_post(&empty);                     /* V(empty): se liberó un espacio */

        printf("    Consumidor %d retiró %3d de la posición %d\n", id, result, pos);
        usleep((rand() % 400) * 1000);        /* consume data (simulado)  */
    }

    printf("    Consumidor %d terminó.\n", id);
    return NULL;
}

/*  main */
int main(void)
{
    pthread_t prod[NUM_PRODUCTORES], cons[NUM_CONSUMIDORES];
    int id_prod[NUM_PRODUCTORES], id_cons[NUM_CONSUMIDORES];

    srand(42);


    sem_init(&empty,  SHARED, N);   
    sem_init(&full,   SHARED, 0);   
    sem_init(&mutexP, SHARED, 1);
    sem_init(&mutexC, SHARED, 1);

    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        id_prod[i] = i + 1;
        if (pthread_create(&prod[i], NULL, productor, &id_prod[i]) != 0) {
            perror("pthread_create productor");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        id_cons[i] = i + 1;
        if (pthread_create(&cons[i], NULL, consumidor, &id_cons[i]) != 0) {
            perror("pthread_create consumidor");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_PRODUCTORES; i++)  pthread_join(prod[i], NULL);
    for (int i = 0; i < NUM_CONSUMIDORES; i++) pthread_join(cons[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutexP);
    sem_destroy(&mutexC);

    printf("\nFin: se produjeron y consumieron %d elementos.\n", TOTAL_ITEMS);
    return EXIT_SUCCESS;
}