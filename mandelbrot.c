#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
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

int calcular_mandelbrot_bruto(int x, int largura, int y, int altura, int max_iter) {

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

    return iteracoes;
}

void *calcular_mandelbrot_pthreads1(void *arg) {
    Pthreads *args = (Pthreads *)arg;

    for (int y = args->linha_inicio; y < args->linha_fim; y++) {
        for (int x = 0; x < args->largura; x++) {
            int intensidade = calcular_pixel_mandelbrot(x, args->largura, y, args->altura, args->max_iteracoes);
            args->matriz_resultado[y * args->largura + x] = intensidade;
        }
    }
    return NULL;
}

void *normalizar_matriz_pthreads2(void *arg) {
    Pthreads *args = (Pthreads *)arg;

    for (int i = args->linha_inicio; i < args->linha_fim; i++) {
        int iteracoes_brutas = args->matriz_resultado[i];
        
        args->matriz_resultado[i] = (iteracoes_brutas * 255) / args->max_iteracoes;
    }
    return NULL;
}
