#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Esta función encuentra el "bin" correspondiente para un dato y aumenta su contador
void encontrar_bin(float dato, float medida_minima, float ancho_bin, int cantidad_bins, int *contadores_bins) {
    int bin = (int)((dato - medida_minima) / ancho_bin);
    if (bin >= 0 && bin < cantidad_bins) {
        contadores_bins[bin]++;
    }
}

int main(int argc, char *argv[]) {
    int cantidad_datos = 20; // Número total de datos
    float datos[20] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 
                       4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
    float medida_minima = 0.0, medida_maxima = 5.0; // Rango de medidas
    int cantidad_bins = 5; // Cantidad de bins para el histograma

    int rango, tamaño; // Variables para el rango del proceso y el tamaño total
    MPI_Init(&argc, &argv); // Inicializar MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rango); // Obtener el rango del proceso actual
    MPI_Comm_size(MPI_COMM_WORLD, &tamaño); // Obtener el número total de procesos

    int cantidad_datos_local = cantidad_datos / tamaño; // Cantidad de datos que maneja cada proceso
    float ancho_bin = (medida_maxima - medida_minima) / cantidad_bins; // Calcular el ancho de cada bin

    // Inicializar contadores de bins para el proceso local
    int *contadores_bins_locales = (int *)calloc(cantidad_bins, sizeof(int));
    int *contadores_bins_globales = NULL; // Para almacenar el resultado global
    if (rango == 0) {
        contadores_bins_globales = (int *)calloc(cantidad_bins, sizeof(int));
    }

    // Distribuir los datos entre los procesos
    float *datos_locales = (float *)malloc(cantidad_datos_local * sizeof(float));
    MPI_Scatter(datos, cantidad_datos_local, MPI_FLOAT, datos_locales, cantidad_datos_local, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Calcular el histograma local
    for (int i = 0; i < cantidad_datos_local; i++) {
        encontrar_bin(datos_locales[i], medida_minima, ancho_bin, cantidad_bins, contadores_bins_locales);
    }

    // Sumar los histogramas locales en el proceso 0
    MPI_Reduce(contadores_bins_locales, contadores_bins_globales, cantidad_bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Mostrar el histograma final en el proceso 0
    if (rango == 0) {
        printf("Histograma final:\n");
        for (int i = 0; i < cantidad_bins; i++) {
            printf("Bin %d (%.1f - %.1f): %d\n", i, medida_minima + i * ancho_bin, medida_minima + (i + 1) * ancho_bin, contadores_bins_globales[i]);
        }
        free(contadores_bins_globales); // Liberar memoria
    }

    free(datos_locales); // Liberar memoria
    free(contadores_bins_locales); // Liberar memoria
    MPI_Finalize(); // Finalizar MPI
    return 0;
}
