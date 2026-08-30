#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stddef.h>

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

void calcular_serial(
    int *imagem,
    int largura,
    int altura,
    int max_iteracoes
);

int normalizar_intensidade(
    int iteracoes,
    int max_iteracoes
);

void normalizar_imagem(
    int *imagem,
    size_t total_pixels,
    int max_iteracoes
);

void calcular_openmp(
    int *imagem,
    int largura,
    int altura,
    int max_iteracoes,
    int num_threads
);

int calcular_pthreads1(
    int *imagem,
    int largura,
    int altura,
    int max_iteracoes,
    int num_threads
);

#endif