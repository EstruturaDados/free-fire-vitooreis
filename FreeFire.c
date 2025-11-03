#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_NOME 30
#define MAX_TIPO 20
#define MAX_COMP 20

// ====================== STRUCT ======================

typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int prioridade; // 1 a 10 (1 = mais crítica)
} Componente;

// ====================== UTILITÁRIOS ======================

void limparEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void trimNovaLinha(char *s) {
    size_t n = strlen(s);
    if (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) s[n-1] = '\0';
}

void lerLinha(const char *prompt, char *buffer, size_t tam) {
    printf("%s", prompt);
    if (fgets(buffer, (int)tam, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    trimNovaLinha(buffer);
}

int lerInteiro(const char *prompt, int *dest) {
    printf("%s", prompt);
    int ok = scanf("%d", dest);
    limparEntrada();
    return ok == 1;
}

// Comparação de strings sem diferenciar maiúsculas/minúsculas
int comparaIgnoreCase(const char *a, const char *b) {
    unsigned char ca, cb;
    while (*a && *b) {
        ca = (unsigned char)tolower((unsigned char)*a);
        cb = (unsigned char)tolower((unsigned char)*b);
        if (ca != cb) return (int)ca - (int)cb;
        a++; b++;
    }
    ca = (unsigned char)tolower((unsigned char)*a);
    cb = (unsigned char)tolower((unsigned char)*b);
    return (int)ca - (int)cb;
}

// Lê um componente do usuário
Componente lerComponente(void) {
    Componente c;

    lerLinha("Nome do componente: ", c.nome, sizeof(c.nome));
    while (strlen(c.nome) == 0) {
        printf("Nome não pode ser vazio.\n");
        lerLinha("Nome do componente: ", c.nome, sizeof(c.nome));
    }

    lerLinha("Tipo (controle, suporte, propulsao, etc.): ", c.tipo, sizeof(c.tipo));
    if (strlen(c.tipo) == 0) {
        strcpy(c.tipo, "indefinido");
    }

    while (!lerInteiro("Prioridade (1 a 10, onde 1 é mais crítica): ", &c.prioridade)
           || c.prioridade < 1 || c.prioridade > 10) {
        printf("Prioridade inválida. Digite um valor entre 1 e 10.\n");
    }

    return c;
}

// Mostra o vetor de componentes
void mostrarComponentes(Componente v[], int n) {
    printf("\n===== COMPONENTES DA TORRE =====\n");
    if (n == 0) {
        printf("(nenhum componente cadastrado)\n");
    } else {
        for (int i = 0; i < n; i++) {
            printf("%2d) Nome: %-20s | Tipo: %-15s | Prioridade: %d\n",
                   i + 1, v[i].nome, v[i].tipo, v[i].prioridade);
        }
    }
    printf("================================\n\n");
}

// ====================== ORDENAÇÕES ======================

// Bubble Sort por NOME
void bubbleSortNome(Componente v[], int n, long *comparacoes) {
    *comparacoes = 0;
    if (n <= 1) return;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            (*comparacoes)++;
            if (comparaIgnoreCase(v[j].nome, v[j + 1].nome) > 0) {
                Componente tmp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = tmp;
            }
        }
    }
}

// Insertion Sort por TIPO
void insertionSortTipo(Componente v[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 1; i < n; i++) {
        Componente chave = v[i];
        int j = i - 1;

        while (j >= 0) {
            (*comparacoes)++;
            if (comparaIgnoreCase(v[j].tipo, chave.tipo) > 0) {
                v[j + 1] = v[j];
                j--;
            } else {
                break;
            }
        }
        v[j + 1] = chave;
    }
}

// Selection Sort por PRIORIDADE (int)
void selectionSortPrioridade(Componente v[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < n - 1; i++) {
        int idxMenor = i;
        for (int j = i + 1; j < n; j++) {
            (*comparacoes)++;
            if (v[j].prioridade < v[idxMenor].prioridade) {
                idxMenor = j;
            }
        }
        if (idxMenor != i) {
            Componente tmp = v[i];
            v[i] = v[idxMenor];
            v[idxMenor] = tmp;
        }
    }
}

// ====================== BUSCA BINÁRIA ======================

int buscaBinariaPorNome(Componente v[], int n, const char *alvo, long *comparacoes) {
    int ini = 0, fim = n - 1;
    *comparacoes = 0;

    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        (*comparacoes)++;
        int cmp = comparaIgnoreCase(alvo, v[meio].nome);

        if (cmp == 0) return meio;
        if (cmp < 0) fim = meio - 1;
        else ini = meio + 1;
    }
    return -1;
}

// ====================== MEDIÇÃO DE TEMPO ======================

typedef void (*SortFunc)(Componente[], int, long*);

// Função genérica para medir tempo de um algoritmo de ordenação
double medirTempoOrdenacao(SortFunc f, Componente v[], int n, long *comparacoes) {
    clock_t inicio = clock();
    f(v, n, comparacoes);
    clock_t fim = clock();
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

// ====================== MAIN / MENU ======================

int main(void) {
    Componente componentes[MAX_COMP];
    int qtd = 0;
    int opcao;

    printf("=== TORRE DE FUGA - DESAFIO FINAL ===\n");
    printf("Monte e priorize os componentes da torre de resgate!\n\n");

    do {
        printf("MENU PRINCIPAL\n");
        printf("1 - Cadastrar componente\n");
        printf("2 - Listar componentes\n");
        printf("3 - Ordenar por NOME (Bubble Sort)\n");
        printf("4 - Ordenar por TIPO (Insertion Sort)\n");
        printf("5 - Ordenar por PRIORIDADE (Selection Sort)\n");
        printf("6 - Buscar componente-chave por NOME (Busca Binária)\n");
        printf("0 - Sair\n");
        printf("==============================\n");

        if (!lerInteiro("Escolha uma opção: ", &opcao)) {
            printf("Entrada inválida.\n");
            continue;
        }

        if (opcao == 1) {
            if (qtd >= MAX_COMP) {
                printf("Limite de %d componentes atingido.\n", MAX_COMP);
            } else {
                componentes[qtd] = lerComponente();
                qtd++;
            }

        } else if (opcao == 2) {
            mostrarComponentes(componentes, qtd);

        } else if (opcao == 3) {
            if (qtd == 0) {
                printf("Cadastre componentes antes de ordenar.\n");
                continue;
            }
            long comp;
            double tempo = medirTempoOrdenacao(bubbleSortNome, componentes, qtd, &comp);
            printf("[BubbleSort - Nome] Comparações: %ld | Tempo: %.6f s\n", comp, tempo);
            mostrarComponentes(componentes, qtd);

        } else if (opcao == 4) {
            if (qtd == 0) {
                printf("Cadastre componentes antes de ordenar.\n");
                continue;
            }
            long comp;
            double tempo = medirTempoOrdenacao(insertionSortTipo, componentes, qtd, &comp);
            printf("[InsertionSort - Tipo] Comparações: %ld | Tempo: %.6f s\n", comp, tempo);
            mostrarComponentes(componentes, qtd);

        } else if (opcao == 5) {
            if (qtd == 0) {
                printf("Cadastre componentes antes de ordenar.\n");
                continue;
            }
            long comp;
            double tempo = medirTempoOrdenacao(selectionSortPrioridade, componentes, qtd, &comp);
            printf("[SelectionSort - Prioridade] Comparações: %ld | Tempo: %.6f s\n", comp, tempo);
            mostrarComponentes(componentes, qtd);

        } else if (opcao == 6) {
            if (qtd == 0) {
                printf("Cadastre componentes antes da busca.\n");
                continue;
            }

            // Garante que está ordenado por nome antes da busca binária
            long compOrd;
            double tempoOrd = medirTempoOrdenacao(bubbleSortNome, componentes, qtd, &compOrd);
            printf("[Pré-processamento] Vetor ordenado por nome antes da busca binária.\n");
            printf("Ordenação - Comparações: %ld | Tempo: %.6f s\n", compOrd, tempoOrd);

            char alvo[MAX_NOME];
            lerLinha("Nome do componente-chave para ativar a torre: ", alvo, sizeof(alvo));

            long compBusca;
            clock_t inicio = clock();
            int idx = buscaBinariaPorNome(componentes, qtd, alvo, &compBusca);
            clock_t fim = clock();
            double tempoBusca = (double)(fim - inicio) / CLOCKS_PER_SEC;

            if (idx >= 0) {
                printf("[ENCONTRADO] Nome: %s | Tipo: %s | Prioridade: %d\n",
                       componentes[idx].nome, componentes[idx].tipo, componentes[idx].prioridade);
                printf("Componente-chave presente! Torre pode ser ativada.\n");
            } else {
                printf("Componente '%s' NÃO encontrado. A torre não pode ser ativada.\n", alvo);
            }
            printf("[Busca Binária] Comparações: %ld | Tempo: %.6f s\n", compBusca, tempoBusca);

        } else if (opcao == 0) {
            printf("Encerrando o sistema. Boa fuga!\n");
        } else {
            printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    return 0;
}
