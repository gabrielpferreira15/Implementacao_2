#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "mandelbrot.h"

long converter_argumento(const char *str, const char *nome_campo) {
    char *endptr;
    errno = 0;
    long valor = strtol(str, &endptr, 10);
    
    if (errno != 0 || *endptr != '\0' || valor <= 0) {
        fprintf(stderr, "Erro: Valor inválido para '%s'. Insira um numero inteiro positivo.\n", nome_campo);
        exit(1);
    }
    return valor;
}

int main(int argc, char *argv[]){

    if (argc != 5) {
        fprintf(stderr, "Erro: Número errado de argumentos\n");
        return 1; 
    }

    int largura = (int)converter_argumento(argv[1], "largura");   
    int altura = (int)converter_argumento(argv[2], "altura");
    int max_iteracoes = (int)converter_argumento(argv[3], "max_iteracoes");
    int num_threads = (int)converter_argumento(argv[4], "num_threads");

    if (largura <= 0 || altura <= 0 || max_iteracoes <= 0) {
        fprintf(stderr, "Erro: Parâmetros inválidos\n");
        return 1;
    }

    FILE *arquivo = fopen("mandelbrot_gpsf_serial.pgm", "w");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Falha ao criar/abrir o arquivo\n");
        return 1;
    }

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            
            int intensidade = calcular_pixel_mandelbrot(x, largura, y, altura, max_iteracoes);
            
            if (fprintf(arquivo, "%d ", intensidade) < 0) {
                fprintf(stderr, "Erro: Falha ao escrever no arquivo\n");
                fclose(arquivo);
                return 1;
            }
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    return 0;
}