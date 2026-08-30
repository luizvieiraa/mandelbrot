#include <errno.h>
#include <limits.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mandelbrot.h"

static int converter_inteiro_positivo(
    const char *texto,
    int *resultado
)
{
    char *fim;
    long valor;

    errno = 0;
    fim = NULL;
    valor = strtol(texto, &fim, 10);

    if (errno == ERANGE || fim == texto || *fim != '\0') {
        return 0;
    }

    if (valor <= 0 || valor > INT_MAX) {
        return 0;
    }

    *resultado = (int)valor;
    return 1;
}

int main(int argc, char *argv[])
{
    int largura;
    int altura;
    int max_iteracoes;
    int num_threads;
    size_t total_pixels;
    int *imagem;

    double inicio;
    double tempo_serial;
    double tempo_openmp;
    double tempo_pthreads1;
    double tempo_pthreads2;

    if (argc != 5) {
        fprintf(
            stderr,
            "Erro: uso correto: %s "
            "[largura] [altura] [max_iteracoes] [num_threads]\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    if (!converter_inteiro_positivo(argv[1], &largura)) {
        fprintf(stderr, "Erro: largura deve ser um inteiro positivo.\n");
        return EXIT_FAILURE;
    }

    if (!converter_inteiro_positivo(argv[2], &altura)) {
        fprintf(stderr, "Erro: altura deve ser um inteiro positivo.\n");
        return EXIT_FAILURE;
    }

    if (
        !converter_inteiro_positivo(
            argv[3],
            &max_iteracoes
        )
    ) {
        fprintf(
            stderr,
            "Erro: max_iteracoes deve ser um inteiro positivo.\n"
        );
        return EXIT_FAILURE;
    }

    if (!converter_inteiro_positivo(argv[4], &num_threads)) {
        fprintf(
            stderr,
            "Erro: num_threads deve ser um inteiro positivo.\n"
        );
        return EXIT_FAILURE;
    }

    if ((size_t)largura > SIZE_MAX / (size_t)altura) {
        fprintf(
            stderr,
            "Erro: dimensoes da imagem sao muito grandes.\n"
        );
        return EXIT_FAILURE;
    }

    total_pixels = (size_t)largura * (size_t)altura;

    if (total_pixels > SIZE_MAX / sizeof(*imagem)) {
        fprintf(
            stderr,
            "Erro: imagem excede o limite de memoria.\n"
        );
        return EXIT_FAILURE;
    }

    imagem = malloc(total_pixels * sizeof(*imagem));

    if (imagem == NULL) {
        fprintf(
            stderr,
            "Erro: falha na alocacao de memoria.\n"
        );
        return EXIT_FAILURE;
    }

    /* Versão serial */

    inicio = omp_get_wtime();

    calcular_serial(
        imagem,
        largura,
        altura,
        max_iteracoes
    );

    tempo_serial = omp_get_wtime() - inicio;

    normalizar_imagem(
        imagem,
        total_pixels,
        max_iteracoes
    );

    if (
        !salvar_imagem(
            "mandelbrot_lhcv_serial.pgm",
            imagem,
            largura,
            altura
        )
    ) {
        fprintf(stderr, "Erro: falha ao salvar imagem serial.\n");
        free(imagem);
        return EXIT_FAILURE;
    }

    /* Versão OpenMP */

    inicio = omp_get_wtime();

    calcular_openmp(
        imagem,
        largura,
        altura,
        max_iteracoes,
        num_threads
    );

    tempo_openmp = omp_get_wtime() - inicio;

    normalizar_imagem(
        imagem,
        total_pixels,
        max_iteracoes
    );

    if (
        !salvar_imagem(
            "mandelbrot_lhcv_openmp.pgm",
            imagem,
            largura,
            altura
        )
    ) {
        fprintf(stderr, "Erro: falha ao salvar imagem OpenMP.\n");
        free(imagem);
        return EXIT_FAILURE;
    }

    /* Versão Pthreads 1 */

    inicio = omp_get_wtime();

    if (
        !calcular_pthreads1(
            imagem,
            largura,
            altura,
            max_iteracoes,
            num_threads
        )
    ) {
        fprintf(
            stderr,
            "Erro: falha na execucao das threads Pthreads 1.\n"
        );
        free(imagem);
        return EXIT_FAILURE;
    }

    tempo_pthreads1 = omp_get_wtime() - inicio;

    normalizar_imagem(
        imagem,
        total_pixels,
        max_iteracoes
    );

    if (
        !salvar_imagem(
            "mandelbrot_lhcv_pthreads1.pgm",
            imagem,
            largura,
            altura
        )
    ) {
        fprintf(stderr, "Erro: falha ao salvar imagem Pthreads 1.\n");
        free(imagem);
        return EXIT_FAILURE;
    }

    /* Versão Pthreads 2 */

    inicio = omp_get_wtime();

    if (
        !calcular_pthreads2(
            imagem,
            largura,
            altura,
            max_iteracoes,
            num_threads
        )
    ) {
        fprintf(
            stderr,
            "Erro: falha na execucao das threads Pthreads 2.\n"
        );
        free(imagem);
        return EXIT_FAILURE;
    }

    tempo_pthreads2 = omp_get_wtime() - inicio;

    normalizar_imagem(
        imagem,
        total_pixels,
        max_iteracoes
    );

    if (
        !salvar_imagem(
            "mandelbrot_lhcv_pthreads2.pgm",
            imagem,
            largura,
            altura
        )
    ) {
        fprintf(stderr, "Erro: falha ao salvar imagem Pthreads 2.\n");
        free(imagem);
        return EXIT_FAILURE;
    }

    if (
        !salvar_tempos(
            tempo_serial,
            tempo_openmp,
            tempo_pthreads1,
            tempo_pthreads2
        )
    ) {
        fprintf(stderr, "Erro: falha ao salvar times.txt.\n");
        free(imagem);
        return EXIT_FAILURE;
    }

    free(imagem);
    return EXIT_SUCCESS;
}