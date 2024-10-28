#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void mezclar(int *array, int *izquierda, int tamañoIzquierda, int *derecha, int tamañoDerecha) {
    int i = 0, j = 0, k = 0;
    
    while (i < tamañoIzquierda && j < tamañoDerecha) {
        if (izquierda[i] <= derecha[j]) {
            array[k++] = izquierda[i++];
        } else {
            array[k++] = derecha[j++];
        }
    }

    while (i < tamañoIzquierda) {
        array[k++] = izquierda[i++];
    }
    while (j < tamañoDerecha) {
        array[k++] = derecha[j++];
    }
}

void ordenamientoPorMezcla(int *array, int tamaño) {
    if (tamaño < 2) return;

    int mitad = tamaño / 2;
    int *izquierda = (int *)malloc(mitad * sizeof(int));
    int *derecha = (int *)malloc((tamaño - mitad) * sizeof(int));

    for (int i = 0; i < mitad; i++) {
        izquierda[i] = array[i];
    }
    for (int i = mitad; i < tamaño; i++) {
        derecha[i - mitad] = array[i];
    }

    ordenamientoPorMezcla(izquierda, mitad);
    ordenamientoPorMezcla(derecha, tamaño - mitad);
    mezclar(array, izquierda, mitad, derecha, tamaño - mitad);

    free(izquierda);
    free(derecha);
}

int main(int argc, char *argv[]) {
    int rango, tamaño;
    int n;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamaño);

    if (rango == 0) {
        printf("Introduce el número de enteros a ordenar (n): ");
        scanf("%d", &n);
    }
    
    // Difundir el tamaño del array a todos los procesos
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int tamañoLocal = n / tamaño;
    int *arrayLocal = (int *)malloc(tamañoLocal * sizeof(int));

    // Inicializar la lista local con números aleatorios
    srand(rango); // Semilla aleatoria basada en el rango del proceso
    for (int i = 0; i < tamañoLocal; i++) {
        arrayLocal[i] = rand() % 100; // Generar números aleatorios entre 0 y 99
    }

    // Ordenar la lista local
    ordenamientoPorMezcla(arrayLocal, tamañoLocal);

    // Imprimir listas locales ordenadas
    printf("Listas locales ordenadas:\n");
    printf("Proceso %d: ", rango);
    for (int i = 0; i < tamañoLocal; i++) {
        printf("%d ", arrayLocal[i]);
    }
    printf("\n");

    // Reunir los arrays locales en el proceso 0
    int *arrayGlobal = NULL;
    if (rango == 0) {
        arrayGlobal = (int *)malloc(n * sizeof(int));
    }
    
    MPI_Gather(arrayLocal, tamañoLocal, MPI_INT, arrayGlobal, tamañoLocal, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Proceso 0 fusiona los resultados
    if (rango == 0) {
        ordenamientoPorMezcla(arrayGlobal, n);
        printf("Array final ordenado:\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", arrayGlobal[i]);
        }
        printf("\n");
        free(arrayGlobal);
    }

    free(arrayLocal);
    MPI_Finalize();
    return 0;
}
