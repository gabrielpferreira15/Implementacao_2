#include <omp.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include "mandelbrot.h"

int converter_argumento(const char *str, const char *nome_campo) {
    char *endptr;
    errno = 0;
    long valor = strtol(str, &endptr, 10);
    
    if (errno != 0 || *endptr != '\0' || valor <= 0 || valor > INT_MAX) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Valor inválido para '%s'. Insira um número inteiro positivo\n", nome_campo);
            fclose(erro);
        }
        exit(1);
    }
    return (int)valor;
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

    int largura = converter_argumento(argv[1], "largura");   
    int altura = converter_argumento(argv[2], "altura");
    int max_iteracoes = converter_argumento(argv[3], "max_iteracoes");
    int num_threads = converter_argumento(argv[4], "num_threads");

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

    int *matriz_imagem = malloc(largura * altura * sizeof(int));
    if (matriz_imagem == NULL) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha de alocação de memória para o Pthreads1\n");
            fclose(erro);
        }
        return 1;
    }

    pthread_t threads[num_threads];
    Pthreads args_thread[num_threads];

    int linhas_por_thread = altura/num_threads;
    int linhas_extras = altura%num_threads;
    int linha_atual = 0;

    for (int i = 0; i < num_threads; i++) {
        args_thread[i].id_thread = i;
        args_thread[i].largura = largura;
        args_thread[i].altura = altura;
        args_thread[i].max_iteracoes = max_iteracoes;
        args_thread[i].matriz_resultado = matriz_imagem;
        args_thread[i].linha_inicio = linha_atual;
        
        int carga_adicional; 

        if (i < linhas_extras) { 
            carga_adicional = 1; 
        } else { 
            carga_adicional = 0; 
        }

        args_thread[i].linha_fim = linha_atual + linhas_por_thread + carga_adicional;
        
        linha_atual = args_thread[i].linha_fim;

        pthread_create(&threads[i], NULL, calcular_mandelbrot_pthreads1, &args_thread[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    FILE *arquivo_pth1 = fopen("mandelbrot_gpsf_pthreads1.pgm", "w");
    if (arquivo_pth1 == NULL) {
        free(matriz_imagem);
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha ao criar o arquivo Pthreads1\n");
            fclose(erro);
        }
        return 1;
    }
    
    int erro_pth1 = 0;
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            if (fprintf(arquivo_pth1, "%d ", matriz_imagem[y * largura + x]) < 0) {
                erro_pth1 = 1;
                break;
            }
        }
        if (erro_pth1 || fprintf(arquivo_pth1, "\n") < 0) {
            erro_pth1 = 1;
            break;
        }
    }

    fclose(arquivo_pth1);
    free(matriz_imagem);

    if (erro_pth1) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha ao escrever no arquivo Pthreads1\n");
            fclose(erro);
        }
        return 1;
    }

    int *matriz_imagem2 = malloc(largura * altura * sizeof(int));
    if (matriz_imagem2 == NULL) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha de alocação de memória para o Pthreads 2\n");
            fclose(erro);
        }
        return 1;
    }

    pthread_t threads2[num_threads];
    Pthreads args_thread2[num_threads];

    for (int i = 0; i < num_threads; i++) {
        args_thread2[i].id_thread = i;
        args_thread2[i].num_threads = num_threads; 
        args_thread2[i].largura = largura;
        args_thread2[i].altura = altura;
        args_thread2[i].max_iteracoes = max_iteracoes;
        args_thread2[i].matriz_resultado = matriz_imagem2;
        
        pthread_create(&threads2[i], NULL, calcular_mandelbrot_pthreads2, &args_thread2[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads2[i], NULL);
    }

    FILE *arquivo_pth2 = fopen("mandelbrot_gpsf_pthreads2.pgm", "w");
    if (arquivo_pth2 == NULL) {
        free(matriz_imagem2);
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha ao criar o arquivo Pthreads 2\n");
            fclose(erro);
        }
        return 1;
    }
    
    int erro_pth2 = 0;
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            if (fprintf(arquivo_pth2, "%d ", matriz_imagem2[y * largura + x]) < 0) {
                erro_pth2 = 1;
                break;
            }
        }
        if (erro_pth2 || fprintf(arquivo_pth2, "\n") < 0) {
            erro_pth2 = 1;
            break;
        }
    }

    fclose(arquivo_pth2);
    free(matriz_imagem2);

    if (erro_pth2) {
        FILE *erro = fopen("erros.txt", "w");
        if (erro != NULL) {
            fprintf(erro, "Erro: Falha ao escrever no arquivo Pthreads 2\n");
            fclose(erro);
        }
        return 1;
    }

    return 0;
}