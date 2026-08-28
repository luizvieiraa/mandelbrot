#include "mandelbrot.h"

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