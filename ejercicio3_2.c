#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int total_lanzamientos;  // Total de lanzamientos
    int rango_proceso, num_procesos;  // Rango del proceso y número de procesos
    long long aciertos_locales = 0, aciertos_globales = 0;  // Conteo de aciertos locales y globales
    long long lanzamientos_locales;  // Lanzamientos para el proceso local
    double x, y, distancia_cuadrada, estimacion_pi;  // Variables para coordenadas y cálculo de pi

    // Inicializamos MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango_proceso);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos);

    if (rango_proceso == 0) {
        total_lanzamientos = 200000000;
        // printf("Ingrese el número total de lanzamientos: ");
        // scanf("%d", &total_lanzamientos);
    }

    // El proceso 0 transmite el número total de lanzamientos a todos los procesos
    MPI_Bcast(&total_lanzamientos, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular lanzamientos locales para cada proceso
    lanzamientos_locales = total_lanzamientos / num_procesos;

    // Semilla de números aleatorios basada en el tiempo y el rango del proceso
    srand(time(NULL) + rango_proceso);

    // Conteo de lanzamientos que caen dentro del círculo
    for (long long lanzamiento = 0; lanzamiento < lanzamientos_locales; lanzamiento++) {
        x = (double)rand() / RAND_MAX * 2.0 - 1.0;  // Coordenada x aleatoria entre -1 y 1
        y = (double)rand() / RAND_MAX * 2.0 - 1.0;  // Coordenada y aleatoria entre -1 y 1
        distancia_cuadrada = x * x + y * y;
        
        if (distancia_cuadrada <= 1) {
            aciertos_locales++;
        }
    }

    // Reducimos el número de lanzamientos en el círculo de todos los procesos a uno solo (proceso 0)
    MPI_Reduce(&aciertos_locales, &aciertos_globales, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rango_proceso == 0) {
        // Estimación de pi usando la fórmula de Monte Carlo
        estimacion_pi = 4 * ((double)aciertos_globales / (double)total_lanzamientos);
        printf("Estimación de pi = %f\n", estimacion_pi);
    }

    // Finalizamos MPI
    MPI_Finalize();
    return 0;
}
