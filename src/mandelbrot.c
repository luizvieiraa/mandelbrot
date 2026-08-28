#include "mandelbrot.h"
#include <omp.h>

#define REAL_MINIMO (-2.0)
#define REAL_MAXIMO 1.0
#define IMAGINARIO_MINIMO (-1.5)
#define IMAGINARIO_MAXIMO 1.5
#define LIMITE_ESCAPE 4.0

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