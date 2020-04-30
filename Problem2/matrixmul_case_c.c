#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main()
{
    FILE* fptr;
    fptr = fopen("tests/input.txt","r");
    if (fptr==NULL)
    {
        printf("input.txt does not exist");
        exit(0);
    }

    // Find the number of rows and columns of first array.
    int n,m;
    fscanf(fptr,"%d %d",&n,&m);

    int *arr1 = malloc(n * m * sizeof(int));

    // Take array input
    for (int i = 0; i < n*m; i++)
        fscanf(fptr,"%d",&arr1[i]);

    // Find the number of rows and columns of second array.
    int p,q;
    fscanf(fptr,"%d %d",&p,&q);
    int *arr2 = malloc(p * q * sizeof(int));

    if (m!=p)
    {
        printf("The matrices aren't the right dimensions");
        exit(0);
    }

    for(int i = 0; i < p; i++)
    {
        for (int j = 0; j < q; j++)
            fscanf(fptr,"%d",&arr2[j*p+i]);
    }

    // Initialize results array.
    int *results = calloc(n*q,sizeof(int));

    double start = omp_get_wtime();

    #pragma omp parallel for collapse(3) schedule(static)
    for (int i = 0; i < n; i++)
    {
        for (int k = 0; k < p; k++)
        {
           for (int j = 0; j < q; j++)
           {
                results[i*q+j] += arr1[i*m+k] * arr2[j*p+k];
           }
        }
    }

    double end = omp_get_wtime();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < q; j++)
            printf("%d\t",results[i*q+j]);
        printf("\n");
    }

    printf("Completed in %f seconds", end - start);
}



