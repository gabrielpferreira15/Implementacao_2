#ifndef MANDELBROT_H
#define MANDELBROT_H

typedef struct {
    int id_thread;
    int largura;
    int altura;
    int max_iteracoes;
    int linha_inicio;
    int linha_fim;
    int *matriz_resultado;
} Pthreads;

int calcular_pixel_mandelbrot(int x, int largura, int y, int altura, int max_iter);

void *calcular_mandelbrot_pthreads(void *arg);

#endif