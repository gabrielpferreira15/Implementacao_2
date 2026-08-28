#include <stdio.h>
#include <stdlib.h>
#include "mandelbrot.h"

int main(int argc, char *argv[]){
if (argc < 5) {
        fprintf(stderr, "Erro: Argumentos insuficientes.\n");
        return 1; 
    }

    int largura = atoi(argv[1]);
    int altura = atoi(argv[2]);
    int max_iteracoes = atoi(argv[3]);

    if (largura <= 0 || altura <= 0 || max_iteracoes <= 0) {
        fprintf(stderr, "Erro: Parametros inválidos.\n");
        return 1;
    }

    FILE *arquivo = fopen("mandelbrot_gpsf_serial.pgm", "w");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Falha ao criar o arquivo.\n");
        return 1;
    }

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            
            int intensidade = calcular_pixel_mandelbrot(x, largura, y, altura, max_iteracoes);
            
            fprintf(arquivo, "%d ", intensidade);
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    return 0;
}