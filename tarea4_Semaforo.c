
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX        10000
#define INICIO_SC  4000
#define FIN_SC     6000
#define ARCHIVO    "salida.txt"

static sem_t sem;   /* semaforo binario: 1 = libre, 0 = ocupado */
static FILE *archivo;   

void *contar(void *arg)
{
    int id = *(int *)arg;

    for (int i = 1; i <= MAX; i++) {
        int en_sc = (i >= INICIO_SC && i <= FIN_SC);

        if (i == INICIO_SC || !en_sc)
            sem_wait(&sem);

        printf("Hilo %d: %d\n", id, i);
        fprintf(archivo, "Hilo %d: %d\n", id, i);

        if (i == FIN_SC || !en_sc)
            sem_post(&sem);
    }
    return NULL;
}

int main(void)
{
    pthread_t h1, h2;
    int id1 = 1, id2 = 2;

    archivo = fopen(ARCHIVO, "w");
    if (archivo == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    if (sem_init(&sem, 0, 1) != 0) {
        perror("sem_init");
        fclose(archivo);
        return EXIT_FAILURE;
    }

    pthread_create(&h1, NULL, contar, &id1);
    pthread_create(&h2, NULL, contar, &id2);

    pthread_join(h1, NULL);
    pthread_join(h2, NULL);

    sem_destroy(&sem);
    printf("Ambos hilos terminaron.\n");
    fprintf(archivo, "Ambos hilos terminaron.\n");
    fclose(archivo);
    printf("Salida guardada en %s\n", ARCHIVO);
    return EXIT_SUCCESS;
}