#include "mandelbrot.h"
#include <omp.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define REAL_MINIMO (-2.0)
#define REAL_MAXIMO 1.0
#define IMAGINARIO_MINIMO (-1.5)
#define IMAGINARIO_MAXIMO 1.5
#define LIMITE_ESCAPE 4.0

typedef struct {
    int *imagem;
    int largura;
    int altura;
    int max_iteracoes;
    int linha_inicial;
    int linha_final;
} ArgumentosPthreads1;

typedef struct {
    int *imagem;
    int largura;
    int altura;
    int max_iteracoes;
    int id_thread;
    int num_threads;
} ArgumentosPthreads2;

int calcular_iteracoes_ponto(
    double real,
    double imaginario,
    int max_iteracoes
)
{
    double z_real = 0.0;
    double z_imaginario = 0.0;
    int iteracao = 0;

    while (
        (z_real * z_real + z_imaginario * z_imaginario)
            <= LIMITE_ESCAPE
        && iteracao < max_iteracoes
    ) {
        double proximo_real =
            z_real * z_real
            - z_imaginario * z_imaginario
            + real;

        double proximo_imaginario =
            2.0 * z_real * z_imaginario
            + imaginario;

        z_real = proximo_real;
        z_imaginario = proximo_imaginario;
        iteracao++;
    }

    return iteracao;
}

int calcular_pixel(
    int coluna,
    int linha,
    int largura,
    int altura,
    int max_iteracoes
)
{
    double proporcao_horizontal = (double)coluna / (double)largura;
    double proporcao_vertical = (double)linha / (double)altura;

    double real =
        REAL_MINIMO
        + proporcao_horizontal * (REAL_MAXIMO - REAL_MINIMO);

    double imaginario =
        IMAGINARIO_MINIMO
        + proporcao_vertical
            * (IMAGINARIO_MAXIMO - IMAGINARIO_MINIMO);

    return calcular_iteracoes_ponto(
        real,
        imaginario,
        max_iteracoes
    );
}

void calcular_serial(
    int *imagem,
    int largura,
    int altura,
    int max_iteracoes
)
{
    for (int linha = 0; linha < altura; linha++) {
        for (int coluna = 0; coluna < largura; coluna++) {
            int indice = linha * largura + coluna;

            imagem[indice] = calcular_pixel(
                coluna,
                linha,
                largura,
                altura,
                max_iteracoes
            );
        }
    }
}

int normalizar_intensidade(
    int iteracoes,
    int max_iteracoes
)
{
    if (iteracoes <= 0) {
        return 0;
    }

    if (iteracoes >= max_iteracoes) {
        return 255;
    }

    return (int)(
        ((long long)iteracoes * 255LL)
        / (long long)max_iteracoes
    );
}

void normalizar_imagem(
    int *imagem,
    size_t total_pixels,
    int max_iteracoes
)
{
    for (size_t indice = 0; indice < total_pixels; indice++) {
        imagem[indice] = normalizar_intensidade(
            imagem[indice],
            max_iteracoes
        );
    }
}

void calcular_openmp(
    int *imagem,
    int largura,
    int altura,
    int max_iteracoes,
    int num_threads
)
{
    #pragma omp parallel for \
        schedule(static) \
        num_threads(num_threads)

    for (int linha = 0; linha < altura; linha++) {
        for (int coluna = 0; coluna < largura; coluna++) {
            int indice = linha * largura + coluna;

            imagem[indice] = calcular_pixel(
                coluna,
                linha,
                largura,
                altura,
                max_iteracoes
            );
        }
    }
}

static void *executar_bloco_pthreads1(void *dados)
{
    ArgumentosPthreads1 *argumentos = dados;

    for (
        int linha = argumentos->linha_inicial;
        linha < argumentos->linha_final;
        linha++
    ) {
        for (
            int coluna = 0;
            coluna < argumentos->largura;
            coluna++
        ) {
            int indice = linha * argumentos->largura + coluna;

            argumentos->imagem[indice] = calcular_pixel(
                coluna,
                linha,
                argumentos->largura,
                argumentos->altura,
                argumentos->max_iteracoes
            );
        }
    }

    return NULL;
}

int calcular_pthreads1(
    int *imagem,
    int largura,
    int altura,
    int max_iteracoes,
    int num_threads
)
{
    pthread_t *threads;
    ArgumentosPthreads1 *argumentos;
    int threads_criadas = 0;
    int ocorreu_erro = 0;

    threads = malloc(
        (size_t)num_threads * sizeof(*threads)
    );

    argumentos = malloc(
        (size_t)num_threads * sizeof(*argumentos)
    );

    if (threads == NULL || argumentos == NULL) {
        free(threads);
        free(argumentos);
        return 0;
    }

    for (int i = 0; i < num_threads; i++) {
        argumentos[i].imagem = imagem;
        argumentos[i].largura = largura;
        argumentos[i].altura = altura;
        argumentos[i].max_iteracoes = max_iteracoes;

        argumentos[i].linha_inicial =
            (i * altura) / num_threads;

        argumentos[i].linha_final =
            ((i + 1) * altura) / num_threads;

        if (
            pthread_create(
                &threads[i],
                NULL,
                executar_bloco_pthreads1,
                &argumentos[i]
            ) != 0
        ) {
            ocorreu_erro = 1;
            break;
        }

        threads_criadas++;
    }

    for (int i = 0; i < threads_criadas; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            ocorreu_erro = 1;
        }
    }

    free(threads);
    free(argumentos);

    return !ocorreu_erro;
}

static void *executar_ciclico_pthreads2(void *dados)
{
    ArgumentosPthreads2 *argumentos = dados;

    for (
        int linha = argumentos->id_thread;
        linha < argumentos->altura;
        linha += argumentos->num_threads
    ) {
        for (
            int coluna = 0;
            coluna < argumentos->largura;
            coluna++
        ) {
            int indice = linha * argumentos->largura + coluna;

            argumentos->imagem[indice] = calcular_pixel(
                coluna,
                linha,
                argumentos->largura,
                argumentos->altura,
                argumentos->max_iteracoes
            );
        }
    }

    return NULL;
}

int calcular_pthreads2(
    int *imagem,
    int largura,
    int altura,
    int max_iteracoes,
    int num_threads
)
{
    pthread_t *threads;
    ArgumentosPthreads2 *argumentos;
    int threads_criadas = 0;
    int ocorreu_erro = 0;

    threads = malloc(
        (size_t)num_threads * sizeof(*threads)
    );

    argumentos = malloc(
        (size_t)num_threads * sizeof(*argumentos)
    );

    if (threads == NULL || argumentos == NULL) {
        free(threads);
        free(argumentos);
        return 0;
    }

    for (int i = 0; i < num_threads; i++) {
        argumentos[i].imagem = imagem;
        argumentos[i].largura = largura;
        argumentos[i].altura = altura;
        argumentos[i].max_iteracoes = max_iteracoes;
        argumentos[i].id_thread = i;
        argumentos[i].num_threads = num_threads;

        if (
            pthread_create(
                &threads[i],
                NULL,
                executar_ciclico_pthreads2,
                &argumentos[i]
            ) != 0
        ) {
            ocorreu_erro = 1;
            break;
        }

        threads_criadas++;
    }

    for (int i = 0; i < threads_criadas; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            ocorreu_erro = 1;
        }
    }

    free(threads);
    free(argumentos);

    return !ocorreu_erro;
}

int salvar_imagem(
    const char *nome_arquivo,
    const int *imagem,
    int largura,
    int altura
)
{
    FILE *arquivo = fopen(nome_arquivo, "w");

    if (arquivo == NULL) {
        return 0;
    }

    for (int linha = 0; linha < altura; linha++) {
        for (int coluna = 0; coluna < largura; coluna++) {
            size_t indice =
                (size_t)linha * (size_t)largura
                + (size_t)coluna;

            if (fprintf(arquivo, "%d", imagem[indice]) < 0) {
                fclose(arquivo);
                return 0;
            }

            if (coluna < largura - 1) {
                if (fputc(' ', arquivo) == EOF) {
                    fclose(arquivo);
                    return 0;
                }
            }
        }

        if (fputc('\n', arquivo) == EOF) {
            fclose(arquivo);
            return 0;
        }
    }

    if (fclose(arquivo) != 0) {
        return 0;
    }

    return 1;
}

int salvar_tempos(
    double serial,
    double openmp,
    double pthreads1,
    double pthreads2
)
{
    FILE *arquivo = fopen("times.txt", "w");
    int ocorreu_erro = 0;

    if (arquivo == NULL) {
        return 0;
    }

    if (
        fprintf(
            arquivo,
            "Serial: %.9f segundos\n"
            "OpenMP: %.9f segundos\n"
            "Pthreads1: %.9f segundos\n"
            "Pthreads2: %.9f segundos\n",
            serial,
            openmp,
            pthreads1,
            pthreads2
        ) < 0
    ) {
        ocorreu_erro = 1;
    }

    if (fclose(arquivo) != 0) {
        ocorreu_erro = 1;
    }

    return !ocorreu_erro;
}