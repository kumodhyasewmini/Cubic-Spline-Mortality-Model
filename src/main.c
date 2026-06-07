#include <stdio.h>
#include <stdlib.h>

void TridiagonalEquationsSolver(int n, double *a, double *b, double *c, double *d, double *x) {
    double alpha[n], beta[n];
    
    // Forward substitution
    alpha[0] = b[0];
    beta[0] = d[0] / b[0];
    
    for (int i = 1; i < n; i++) {
        alpha[i] = b[i] - a[i] * c[i - 1] / alpha[i - 1];
        beta[i] = (d[i] - a[i] * beta[i - 1]) / alpha[i];
    }
    
    // Backward substitution
    x[n - 1] = beta[n - 1];
    for (int i = n - 2; i >= 0; i--) {
        x[i] = beta[i] - c[i] * x[i + 1] / alpha[i];
    }
}

int main() {
    int n;
    
    printf("Enter number of data points: ");
    scanf("%d", &n);
    
    double x[n], y[n], h[n - 1], a[n - 1], b[n], c[n - 1], d[n - 1], M[n];
    
    // Read data points
    for (int i = 0; i < n; i++) {
        printf("Enter x%d and y%d: ", i + 1, i + 1);
        scanf("%lf %lf", &x[i], &y[i]);
    }
    
    // Calculate h[i]
    for (int i = 0; i < n - 1; i++) {
        h[i] = x[i + 1] - x[i];
    }
    
    // Set up the tridiagonal system
    for (int i = 1; i < n - 1; i++) {
        a[i] = h[i - 1];
        b[i] = 2 * (h[i - 1] + h[i]);
        c[i] = h[i];
        d[i] = 6 * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1]);
    }
    
    // Solve the system
    TridiagonalEquationsSolver(n - 2, a + 1, b + 1, c + 1, d + 1, M + 1);
    
    // Set boundary conditions
    M[0] = 0;
    M[n - 1] = 0;
    
    // Open file to save the results
    FILE *fp = fopen("cubic_spline_plot.dat", "w");
    
    // Output the original spline equation and write data points to the file
    double A[n - 1], B[n - 1], C[n - 1], D[n - 1];
    
    for (int i = 0; i < n - 1; i++) {
        A[i] = (M[i + 1] - M[i]) / (6 * h[i]);
        B[i] = M[i] / 2;
        C[i] = (y[i + 1] - y[i]) / h[i] - (M[i + 1] + 2 * M[i]) * h[i] / 6;
        D[i] = y[i];
        
        // Output the cubic spline equations
        printf("S%d(x) = %.4f * (x - %.4f)^3 + %.4f * (x - %.4f)^2 + %.4f * (x - %.4f) + %.4f\n",
               i, A[i], x[i], B[i], x[i], C[i], x[i], D[i]);
        
        // Write the data points for this segment to the file
        double xi = x[i];
        while (xi <= x[i + 1]) {
            double yi = A[i] * (xi - x[i]) * (xi - x[i]) * (xi - x[i])
                      + B[i] * (xi - x[i]) * (xi - x[i])
                      + C[i] * (xi - x[i])
                      + D[i];
            fprintf(fp, "%.4f %.4f\n", xi, yi);
            xi += 0.01;
        }
    }
    
    fclose(fp);
    
    // Final output - transform to a standard polynomial form
    printf("\nFinal output in polynomial form:\n\n");
    for (int i = 0; i < n - 1; i++) {
        double a_poly = A[i];
        double b_poly = B[i] - 3 * A[i] * x[i];
        double c_poly = C[i] + 3 * A[i] * x[i] * x[i] - 2 * B[i] * x[i];
        double d_poly = D[i] - A[i] * x[i] * x[i] * x[i] + B[i] * x[i] * x[i] - C[i] * x[i];
        
        // Output the transformed cubic polynomial
        printf("S%d(x) = %.4f * x^3 + %.4f * x^2 + %.4f * x + %.4f\n",
               i, a_poly, b_poly, c_poly, d_poly);
    }
    
    printf("\nData points have been written to 'cubic_spline_plot.dat'. \n");
    
    // User interaction for substituting x_value
    double x_value, result;
    
    while (1) {
        printf("\nEnter the x value to substitute (or enter -1 to exit): ");
        scanf("%lf", &x_value);
        
        if (x_value == -1) {
            printf("Exiting...\n");
            break;
        }
        
        // Automatically determine which spline equation to use
        int eq_choice = -1;
        for (int i = 0; i < n - 1; i++) {
            if (x_value >= x[i] && x_value <= x[i + 1]) {
                eq_choice = i;
                break;
            }
        }
        
        if (eq_choice == -1) {
            printf("x value is out of range. Please try again.\n");
            continue;
        }
        
        // Evaluate the chosen spline equation
        result = A[eq_choice] * (x_value - x[eq_choice]) * (x_value - x[eq_choice]) * (x_value - x[eq_choice])
               + B[eq_choice] * (x_value - x[eq_choice]) * (x_value - x[eq_choice])
               + C[eq_choice] * (x_value - x[eq_choice])
               + D[eq_choice];
        
        printf("S%d(%4.4f) = %.4f\n", eq_choice, x_value, result);
    }
    
    return 0;
}
