#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   // Include time.h for measuring runtime

// Function to be integrated: f(x) = 4 / (1 + x^2)
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

// Function to perform numerical integration using the trapezoidal rule
double trapezoidal_rule(int num_trapezoids) {
    double a = 0.0;  // Lower limit of integration
    double b = 1.0;  // Upper limit of integration
    double h = (b - a) / num_trapezoids;  // Width of each trapezoid
    double integral = 0.0;

    // Calculate the first and last terms (endpoints)
    integral += 0.5 * (f(a) + f(b));

    // Loop through all intermediate trapezoids
    for (int i = 1; i < num_trapezoids; ++i) {
        double x = a + i * h;  // Calculate x-coordinate of the i-th trapezoid
        integral += f(x);
    }

    // Multiply by the width of the trapezoid
    integral *= h;
    return integral;
}

int main(int argc, char* argv[]) {
    int num_trapezoids;

    // Check if the user provided the -h argument
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        printf("Usage: %s <number_of_trapezoids>\n", argv[0]);
        printf("  -h                 Display this help message and exit.\n");
        printf("  <number_of_trapezoids>  Specify the number of trapezoids for numerical integration.\n");
        return 0;  // Exit without executing the rest of the program
    }

    // Check if the user provided the correct number of arguments
    if (argc != 2) {
        printf("Invalid arguments.\n");
        printf("Use '-h' for help.\n");
        return 1;
    }

    // Convert command-line argument to integer
    num_trapezoids = atoi(argv[1]);

    // Start measuring the computation time
    clock_t start_time = clock();

    // Perform the integration
    double result = trapezoidal_rule(num_trapezoids);

    // Stop measuring the computation time
    clock_t end_time = clock();

    // Calculate elapsed time in milliseconds
    double elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;

    // Display the result and the elapsed time
    printf("The approximate value of the integral is: %.10f\n", result);
    printf("Computation time: %.3f ms\n", elapsed_time);

    return 0;
}
