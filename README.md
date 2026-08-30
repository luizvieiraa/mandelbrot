# Mandelbrot com Paralelismo em C

Implementação do conjunto de Mandelbrot em C, desenvolvida para comparar uma execução serial com três estratégias paralelas: OpenMP e duas versões com Pthreads.

O programa calcula a região real `[-2.0, 1.0]` e imaginária `[-1.5, 1.5]`, normaliza a quantidade de iterações para intensidades entre `0` e `255` e gera quatro arquivos de saída byte a byte idênticos.

## Implementações

- **Serial:** percorre todos os pixels sequencialmente.
- **OpenMP:** distribui estaticamente as linhas entre as threads.
- **Pthreads 1:** divide a imagem em blocos contínuos de linhas.
- **Pthreads 2:** distribui as linhas de forma cíclica entre as threads.

## Requisitos

- Linux, Unix ou macOS
- GCC com suporte a OpenMP
- Biblioteca POSIX Threads
- GNU Make

No Ubuntu ou WSL, as ferramentas podem ser instaladas com:

```bash
sudo apt update
sudo apt install build-essential
```

## Estrutura do projeto

```text
lhcv/
├── src/
│   ├── main.c
│   ├── mandelbrot.c
│   └── mandelbrot.h
├── Makefile
├── README.md
└── evidencias.log
```

## Compilação

```bash
make
```

O Makefile utiliza as opções:

```text
-std=c11 -Wall -Wextra -Wpedantic -O2 -fopenmp -pthread
```

## Execução

```bash
./mandelbrot largura altura max_iteracoes num_threads
```

Exemplo:

```bash
./mandelbrot 800 600 1000 4
```

Também é possível executar o exemplo padrão com:

```bash
make run
```

Durante uma execução normal, o programa não escreve em `stdout`. Mensagens de erro são enviadas para `stderr`.

## Arquivos gerados

Cada execução gera:

```text
mandelbrot_lhcv_serial.pgm
mandelbrot_lhcv_openmp.pgm
mandelbrot_lhcv_pthreads1.pgm
mandelbrot_lhcv_pthreads2.pgm
times.txt
```

Apesar da extensão `.pgm`, os arquivos não possuem cabeçalho. Eles contêm somente intensidades entre `0` e `255`, separadas por espaços, com uma linha para cada linha da imagem.

O arquivo `times.txt` registra separadamente o tempo de cálculo das quatro implementações. Normalização e escrita em disco não fazem parte da medição.

## Verificação dos resultados

Confira a quantidade de linhas:

```bash
wc -l *.pgm
```

Confira se as quatro implementações produziram conteúdo idêntico:

```bash
sha256sum *.pgm
```

Os quatro hashes devem ser iguais.

Confira os tempos:

```bash
cat times.txt
```

Exemplo de formato:

```text
Serial: 0.196747256 segundos
OpenMP: 0.108687254 segundos
Pthreads1: 0.111596103 segundos
Pthreads2: 0.075286970 segundos
```

Os valores variam de acordo com o processador, a carga do sistema e os parâmetros utilizados.

## Tratamento de erros

O programa verifica:

- Quantidade incorreta de argumentos;
- Valores não numéricos, negativos, iguais a zero ou fora do limite de `int`;
- Overflow no cálculo das dimensões e tamanhos de memória;
- Falhas de alocação;
- Falhas na criação e sincronização das Pthreads;
- Falhas na criação, escrita e fechamento dos arquivos.

Em qualquer falha, o programa apresenta uma mensagem coerente em `stderr`, libera os recursos adquiridos e encerra com código diferente de zero.

## Limpeza

Para remover o executável e os arquivos gerados:

```bash
make clean
```

## Autor

**Luiz Vieira**  
Ciência da Computação — CESAR School  
Login acadêmico: `lhcv`

Repositório: [github.com/luizvieiraa/mandelbrot](https://github.com/luizvieiraa/mandelbrot)
