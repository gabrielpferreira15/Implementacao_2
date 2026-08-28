#include <stdio.h>
#include <stdlib.h>
#include "mandelbrot.h"

float min_real = -2.0;
float max_real = 1.0;
float min_imag = -1.5;
float max_imag = 1.5;

int calcular_pixel_mandelbrot(int x, int largura, int y, int altura, int max_iter){

    float real = (max_real - min_real)/largura;
    float imag = (max_imag - min_imag)/altura;

    float c_real = min_real + (x * real);
    float c_imag = min_imag + (y * imag);

    float a = 0.0;
    float b = 0.0;
    int iteracoes = 0;

    while ((a * a + b * b <= 4.0) && (iteracoes < max_iter)){
        float a_novo = (a * a) - (b * b) + c_real;
        float b_novo = 2*a*b + c_imag;

        a = a_novo;
        b = b_novo;
        iteracoes++;
    }

    int intensidade_pixel = (iteracoes * 255)/max_iter;

    return intensidade_pixel;
}

