#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CAPACIDADE 10
#define MAX_NOME 30
#define MAX_TIPO 20

// ====================== STRUCTS BÁSICAS ======================

// Item que representa um loot qualquer
typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int quantidade;
} Item;

// Nó para lista encadeada
typedef struct No {
    Item dados;
    struct No *proximo;
} No;

// ====================== CONTADORES DE BUSCA ======================

long comparacoesSeqVetor = 0;
long comparacoesBinVetor = 0;
long comparacoesSeqLista = 0;

// ====================== UTILITÁRIOS DE ENTRADA ======================

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

// ====================== COMPARAÇÕES DE STRINGS ======================

// Retorna 1 se for igual ignorando maiúsculas/minúsculas
int iguaisIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

// Retorna <0, 0, >0 comparando ignorando maiúsculas/minúsculas
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

// Cria um item lendo do teclado
Item lerItemDoUsuario(void) {
    Item novo;
    lerLinha("Nome do item: ", novo.nome, sizeof(novo.nome));
    while (strlen(novo.nome) == 0) {
        printf("Nome não pode ser vazio.\n");
        lerLinha("Nome do item: ", novo.nome, sizeof(novo.nome));
    }

    lerLinha("Tipo (arma, municao, cura, ferramenta, etc.): ",
             novo.tipo, sizeof(novo.tipo));
    if (strlen(novo.tipo) == 0) {
        strcpy(novo.tipo, "indefinido");
    }

    while (!lerInteiro("Quantidade (>=1): ", &novo.quantidade) ||
           novo.quantidade < 1) {
        printf("Quantidade inválida. Tente novamente.\n");
    }

    return novo;
}

// ====================== VERSÃO COM VETOR ======================

// Insere item no vetor: se nome já existir, soma quantidade.
// Caso contrário, adiciona ao final (se tiver espaço).
int inserirItemVetor(Item mochila[], int *tamanho) {
    if (*tamanho >= CAPACIDADE) {
        printf("[VETOR] Mochila cheia (capacidade %d).\n", CAPACIDADE);
        return 0;
    }

    Item novo = lerItemDoUsuario();

    // Verifica se já existe item com o mesmo nome
    for (int i = 0; i < *tamanho; i++) {
        if (iguaisIgnoreCase(mochila[i].nome, novo.nome)) {
            mochila[i].quantidade += novo.quantidade;
            printf("[VETOR] '%s' já existia. Quantidade somada: agora %d.\n",
                   mochila[i].nome, mochila[i].quantidade);
            return 1;
        }
    }

    mochila[*tamanho] = novo;
    (*tamanho)++;
    printf("[VETOR] Item '%s' adicionado.\n", novo.nome);
    return 1;
}

// Remove item por nome, compactando o vetor
int removerItemVetor(Item mochila[], int *tamanho) {
    if (*tamanho == 0) {
        printf("[VETOR] Mochila vazia. Nada para remover.\n");
        return 0;
    }

    char alvo[MAX_NOME];
    lerLinha("Nome do item a remover: ", alvo, sizeof(alvo));

    for (int i = 0; i < *tamanho; i++) {
        if (iguaisIgnoreCase(mochila[i].nome, alvo)) {
            for (int j = i; j < *tamanho - 1; j++) {
                mochila[j] = mochila[j+1];
            }
            (*tamanho)--;
            printf("[VETOR] Item '%s' removido.\n", alvo);
            return 1;
        }
    }

    printf("[VETOR] Item '%s' não encontrado.\n", alvo);
    return 0;
}

// Lista itens do vetor
void listarItensVetor(const Item mochila[], int tamanho) {
    printf("\n----- INVENTÁRIO (VETOR) -----\n");
    if (tamanho == 0) {
        printf("(vazio)\n");
    } else {
        for (int i = 0; i < tamanho; i++) {
            printf("%2d) Nome: %-20s | Tipo: %-12s | Qtd: %d\n",
                   i + 1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
        }
    }
    printf("Capacidade usada: %d/%d\n", tamanho, CAPACIDADE);
    printf("------------------------------\n\n");
}

// Ordena vetor por nome (Bubble Sort)
void ordenarVetor(Item mochila[], int tamanho) {
    if (tamanho <= 1) {
        printf("[VETOR] Poucos itens, nada para ordenar.\n");
        return;
    }

    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - 1 - i; j++) {
            if (comparaIgnoreCase(mochila[j].nome, mochila[j+1].nome) > 0) {
                Item tmp = mochila[j];
                mochila[j] = mochila[j+1];
                mochila[j+1] = tmp;
            }
        }
    }
    printf("[VETOR] Itens ordenados por nome (Bubble Sort).\n");
}

// Busca sequencial no vetor (conta comparações)
int buscarSequencialVetor(const Item mochila[], int tamanho, const char *alvo) {
    comparacoesSeqVetor = 0; // zera contador

    for (int i = 0; i < tamanho; i++) {
        comparacoesSeqVetor++; // comparação de nome
        if (iguaisIgnoreCase(mochila[i].nome, alvo)) {
            return i;
        }
    }
    return -1;
}

// Busca binária no vetor (supõe vetor já ordenado por nome)
int buscarBinariaVetor(const Item mochila[], int tamanho, const char *alvo) {
    int inicio = 0;
    int fim = tamanho - 1;
    comparacoesBinVetor = 0; // zera contador

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;

        comparacoesBinVetor++; // uma comparação por iteração
        int cmp = comparaIgnoreCase(alvo, mochila[meio].nome);

        if (cmp == 0) {
            return meio;
        } else if (cmp < 0) {
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }
    return -1;
}

// ====================== VERSÃO COM LISTA ENCADEADA ======================

// Insere item na lista (no início).
// Se já existir item com mesmo nome, soma quantidades.
void inserirItemLista(No **lista) {
    Item novo = lerItemDoUsuario();

    // Verifica se já existe
    for (No *p = *lista; p != NULL; p = p->proximo) {
        if (iguaisIgnoreCase(p->dados.nome, novo.nome)) {
            p->dados.quantidade += novo.quantidade;
            printf("[LISTA] '%s' já existia. Quantidade somada: agora %d.\n",
                   p->dados.nome, p->dados.quantidade);
            return;
        }
    }

    No *n = (No*)malloc(sizeof(No));
    if (!n) {
        printf("[LISTA] Erro de alocação de memória.\n");
        return;
    }
    n->dados = novo;
    n->proximo = *lista;
    *lista = n;
    printf("[LISTA] Item '%s' inserido na lista.\n", novo.nome);
}

// Remove item por nome da lista
int removerItemLista(No **lista) {
    if (*lista == NULL) {
        printf("[LISTA] Mochila vazia. Nada para remover.\n");
        return 0;
    }

    char alvo[MAX_NOME];
    lerLinha("Nome do item a remover: ", alvo, sizeof(alvo));

    No *anterior = NULL;
    No *atual = *lista;

    while (atual != NULL) {
        if (iguaisIgnoreCase(atual->dados.nome, alvo)) {
            if (anterior == NULL) {
                *lista = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            free(atual);
            printf("[LISTA] Item '%s' removido.\n", alvo);
            return 1;
        }
        anterior = atual;
        atual = atual->proximo;
    }

    printf("[LISTA] Item '%s' não encontrado.\n", alvo);
    return 0;
}

// Lista itens da lista encadeada
void listarItensLista(const No *lista) {
    printf("\n----- INVENTÁRIO (LISTA) -----\n");
    if (lista == NULL) {
        printf("(vazio)\n");
    } else {
        int i = 1;
        for (const No *p = lista; p != NULL; p = p->proximo, i++) {
            printf("%2d) Nome: %-20s | Tipo: %-12s | Qtd: %d\n",
                   i, p->dados.nome, p->dados.tipo, p->dados.quantidade);
        }
    }
    printf("------------------------------\n\n");
}

// Busca sequencial na lista (conta comparações)
No* buscarSequencialLista(No *lista, const char *alvo) {
    comparacoesSeqLista = 0; // zera contador

    for (No *p = lista; p != NULL; p = p->proximo) {
        comparacoesSeqLista++; // comparação de nome
        if (iguaisIgnoreCase(p->dados.nome, alvo)) {
            return p;
        }
    }
    return NULL;
}

// Libera toda a lista encadeada
void liberarLista(No **lista) {
    No *p = *lista;
    while (p != NULL) {
        No *prox = p->proximo;
        free(p);
        p = prox;
    }
    *lista = NULL;
}

// ====================== MENUS ======================

void menuVetor(Item mochila[], int *tamanho) {
    int opcao;
    do {
        printf("====== MODO VETOR ======\n");
        printf("1 - Inserir item\n");
        printf("2 - Remover item\n");
        printf("3 - Listar itens\n");
        printf("4 - Buscar sequencial por nome\n");
        printf("5 - Ordenar vetor por nome\n");
        printf("6 - Buscar binária por nome (requer vetor ordenado)\n");
        printf("0 - Voltar ao menu principal\n");
        printf("========================\n");

        if (!lerInteiro("Escolha sua opção: ", &opcao)) {
            printf("Entrada inválida.\n");
            continue;
        }

        if (opcao == 1) {
            inserirItemVetor(mochila, tamanho);
            listarItensVetor(mochila, tamanho);
        } else if (opcao == 2) {
            removerItemVetor(mochila, tamanho);
            listarItensVetor(mochila, tamanho);
        } else if (opcao == 3) {
            listarItensVetor(mochila, tamanho);
        } else if (opcao == 4) {
            char alvo[MAX_NOME];
            lerLinha("Nome do item para busca sequencial: ", alvo, sizeof(alvo));
            int idx = buscarSequencialVetor(mochila, *tamanho, alvo);
            if (idx >= 0) {
                printf("[VETOR] ENCONTRADO: Nome: %s | Tipo: %s | Qtd: %d\n",
                       mochila[idx].nome, mochila[idx].tipo, mochila[idx]
