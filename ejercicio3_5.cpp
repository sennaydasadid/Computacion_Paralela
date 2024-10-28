#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 4;  // Tamaño de la matriz (ajustable)

    // Matriz A definida directamente
    double A[16] = {1.0, 2.0, 3.0, 4.0,
                   5.0, 6.0, 7.0, 8.0,
                   9.0, 10.0, 11.0, 12.0,
                   13.0, 14.0, 15.0, 16.0};

    // Vector x definido directamente
    double x[4] = {1.0, 2.0, 3.0, 4.0};

    // Calcular el tamaño de cada bloque
    int block_size = n / size;

    // Alocar memoria para el bloque local de A y la porción local de y
    double* A_local = new double[block_size * n];
    double* y_local = new double[block_size];

    // Dispersar la matriz a todos los procesos
    MPI_Scatter(A, block_size * n, MPI_DOUBLE, A_local, block_size * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Transmitir el vector x a todos los procesos
    MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Realizar la multiplicación matriz-vector local
    for (int i = 0; i < block_size; ++i) {
        y_local[i] = 0.0;
        for (int j = 0; j < n; ++j) {
            y_local[i] += A_local[i * n + j] * x[j];
        }
    }

    // Alocar memoria para el vector y en el proceso 0
    double* y = nullptr;
    if (rank == 0) {
        y = new double[n];
    }

    // Reunir los resultados locales en el proceso 0
    MPI_Gather(y_local, block_size, MPI_DOUBLE, y, block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Imprimir el vector resultado y
        for (int i = 0; i < n; ++i) {
            std::cout << y[i] << " ";
        }
        std::cout << std::endl;

        // Liberar la memoria
        delete[] y;
    }

    // Liberar la memoria
    delete[] A_local;
    delete[] y_local;

    MPI_Finalize();
    return 0;
}

