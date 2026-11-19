#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Declaramos el semaforo de forma global para que ambos hilos lo usen
sem_t semaforo;

// Funcion que van a ejecutar los dos hilos
void* contar(void* arg) {
    int id_hilo = *(int*)arg;

    for (int i = 1; i <= 10000; i++) {
        // Antes de llegar a 4000, pedimos el semaforo para que nadie interrumpa
        if (i == 4000) {
            sem_wait(&semaforo);
            printf("--- Hilo %d ENTRA a la seccion critica (4000 - 6000) ---\n", id_hilo);
        }

        // Si estamos fuera del rango 4000-6000, pedimos el semaforo solo para imprimir
        // y no colarnos mientras el otro hilo esta en su seccion critica
        if (i < 4000 || i > 6000) {
            sem_wait(&semaforo);
            printf("Hilo %d: %d\n", id_hilo, i);
            sem_post(&semaforo);
        } else {
            // Aqui ya tenemos el semaforo tomado desde i == 4000 hasta i == 6000
            printf("Hilo %d: %d\n", id_hilo, i);
        }

        // Al terminar el 6000, liberamos el semaforo para que el otro hilo pueda pasar
        if (i == 6000) {
            printf("--- Hilo %d SALE de la seccion critica ---\n", id_hilo);
            sem_post(&semaforo);
        }
    }

    return NULL;
}

int main() {
    pthread_t hilo1, hilo2;
    int id1 = 1, id2 = 2;

    // Inicializamos el semaforo en 1 (semaforo binario)
    // El 0 indica que se comparte entre hilos del mismo proceso
    if (sem_init(&semaforo, 0, 1) != 0) {
        printf("Error al inicializar el semaforo\n");
        return 1;
    }

    // Creamos los dos hilos
    pthread_create(&hilo1, NULL, contar, &id1);
    pthread_create(&hilo2, NULL, contar, &id2);

    // Esperamos a que ambos hilos terminen su ejecucion
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    // Destruimos el semaforo al terminar
    sem_destroy(&semaforo);

    printf("Programa terminado correctamente.\n");
    return 0;
}
