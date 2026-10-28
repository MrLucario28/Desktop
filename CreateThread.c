#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* contar_decreciente(void* arg) {
    for (int i = 10000; i >= 1; i--) {
        printf("[Hilo Secundario]: %d\n", i);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hilo_id;
    int resultado;

    resultado = pthread_create(&hilo_id, NULL, contar_decreciente, NULL);
    if (resultado != 0) {
        perror("Error al crear el hilo");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= 10000; i++) {
        printf("<Proceso Principal>: %d\n", i);
    }

    pthread_join(hilo_id, NULL);

    printf("Ambos procesos han finalizado con éxito.\n");
    return 0;
}