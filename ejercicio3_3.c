#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int rango, tamaño_comunicacion;
    int suma_local, suma_global;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamaño_comunicacion);

    // Cada proceso asigna un valor inicial a `suma_local` (puede ser el propio rango por simplicidad)
    suma_local = rango;

    int paso = 1;

    // Reducción en un árbol (tree-structured reduction)
    while (paso < tamaño_comunicacion) {
        if (rango % (2 * paso) == 0) {
            int valor_recibido;
            int fuente = rango + paso;

            // Asegurarse de que la fuente no esté fuera de rango
            if (fuente < tamaño_comunicacion) {
                MPI_Recv(&valor_recibido, 1, MPI_INT, fuente, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                suma_local += valor_recibido;
            }
        } else if ((rango - paso) % (2 * paso) == 0) {
            MPI_Send(&suma_local, 1, MPI_INT, rango - paso, 0, MPI_COMM_WORLD);
            break;
        }
        paso *= 2;
    }

    // Proceso 0 contiene el resultado de la suma global
    if (rango == 0) {
        suma_global = suma_local;
        printf("La suma global es: %d\n", suma_global);
    }

    MPI_Finalize();
    return 0;
}
