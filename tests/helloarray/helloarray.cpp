#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <iostream>

int main (void) {

    //
    //
    //
    char* array = (char*)malloc(256);

    for (int i=0; i<256; i++) {
        array[i] = i;
    }

    //
    //
    //
    int* int_array = (int*)malloc(256);

    for (int i=0; i<64; i++) {
        int_array[i] = i*0;
    }

    //
    //
    //
    float* float_array = (float*)malloc(256);

    for (int i=0; i<64; i++) {
        float_array[i] = (float)i * 1.13;
    }

    //
    //
    //
    int    table_size     = 512;
    double two_pi         = (3.14159 * 2);
    float  phaseIncrement = two_pi/table_size;
    float  currentPhase   = 0.0;

    float* sine_array = (float*)malloc(table_size*sizeof(float));

    for (int i = 0; i < 512; i ++){

        sine_array[i] = sin(currentPhase);

        currentPhase += phaseIncrement;
    }


    //
    //
    //
    printf("Hello array!\n");

    free(array);
    free(int_array);
    free(float_array);
    free(sine_array);

    return 0;
}

