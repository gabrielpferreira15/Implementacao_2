#include <omp.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "mandelbrot.h"

long converter_argumento(const char *str, const char *nome_campo) {
    char *endptr;
    errno = 0;
    long valor = strtol(str, &endptr, 10);
    
    if (errno != 0 || *endptr != '\0' || valor <= 0) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Valor invalido para '%s'. Insira um numero inteiro positivo\n", nome_campo);
            fclose(erro);
        }
        exit(1);
    }
    return valor;
}

int main(int argc, char *argv[]){

    if (argc != 5) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Número errado de argumentos\n");
            fclose(erro);
        }
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
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha ao criar o arquivo serial\n");
            fclose(erro);
        }
        return 1;
    }

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            
            int intensidade = calcular_pixel_mandelbrot(x, largura, y, altura, max_iteracoes);
            
            if (fprintf(arquivo, "%d ", intensidade) < 0) {
                fclose(arquivo);
                FILE *erro = fopen("erros.txt", "w");
                if (erro != NULL) {
                    fprintf(erro, "Erro: Falha ao escrever no arquivo serial\n");
                    fclose(erro);
                }
                return 1;
            }
        }
        if (fprintf(arquivo, "\n") < 0) {
            fclose(arquivo);
            FILE *erro = fopen("erros.txt", "w");
            if (erro != NULL) {
                fprintf(erro, "Erro: Falha ao escrever no arquivo serial\n");
                fclose(erro);
            }
            return 1;
        }
    }
    fclose(arquivo);

    FILE *arquivo_omp = fopen("mandelbrot_gpsf_openmp.pgm", "w");

    if (arquivo_omp == NULL) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha ao criar o arquivo OpenMP\n");
            fclose(erro);
        }
        return 1;
    }

    omp_set_num_threads(num_threads);

    int erro_omp = 0;

    #pragma omp parallel for ordered shared(erro_omp)
    for (int y = 0; y < altura; y++) {

        if (erro_omp){
            continue;
        }

        int buffer[largura];
        for (int x = 0; x < largura; x++) {
            buffer[x] = calcular_pixel_mandelbrot(x, largura, y, altura, max_iteracoes);
        }

        #pragma omp ordered
        {
            if (!erro_omp) {
                for (int x = 0; x < largura; x++) {
                    if (fprintf(arquivo_omp, "%d ", buffer[x]) < 0) {
                        erro_omp = 1;
                    }
                }
                if (fprintf(arquivo_omp, "\n") < 0) {
                    erro_omp = 1;
                }
            }
        }
    }
    fclose(arquivo_omp);

    if (erro_omp) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha ao escrever no arquivo OpenMP\n");
            fclose(erro);
        }
        return 1;
    }

    return 0;
}