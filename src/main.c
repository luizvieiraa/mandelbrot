#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int converter_inteiro_positivo(const char *texto, int *resultado)
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

    if (!converter_inteiro_positivo(argv[3], &max_iteracoes)) {
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
        fprintf(stderr, "Erro: dimensoes da imagem sao muito grandes.\n");
        return EXIT_FAILURE;
    }

    /*
     * A execução das quatro implementações será adicionada
     * nas próximas etapas.
     */
    return EXIT_SUCCESS;
}