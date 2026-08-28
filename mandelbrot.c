#include <stdio.h>
#include <stdlib.h>
#include "mandelbrot.h"

double min_real = -2.0;
double max_real = 1.0;
double min_imag = -1.5;
double max_imag = 1.5;

int calcular_pixel_mandelbrot(int x, int largura, int y, int altura, int max_iter){

    double real = (max_real - min_real)/largura;
    double imag = (max_imag - min_imag)/altura;

    double c_real = min_real + (x * real);
    double c_imag = min_imag + (y * imag);

    double a = 0.0;
    double b = 0.0;
    int iteracoes = 0;

    while ((a * a + b * b <= 4.0) && (iteracoes < max_iter)){
        double a_novo = (a * a) - (b * b) + c_real;
        double b_novo = 2*a*b + c_imag;

        a = a_novo;
        b = b_novo;
        iteracoes++;
    }

    int intensidade_pixel = (iteracoes * 255)/max_iter;

    return intensidade_pixel;
}

