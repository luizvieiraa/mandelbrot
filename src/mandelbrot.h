#ifndef MANDELBROT_H
#define MANDELBROT_H

int calcular_iteracoes_ponto(
    double real,
    double imaginario,
    int max_iteracoes
);

int calcular_pixel(
    int coluna,
    int linha,
    int largura,
    int altura,
    int max_iteracoes
);

#endif