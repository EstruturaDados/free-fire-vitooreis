#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define CAPACIDADE 10
#define MAX_NOME 30
#define MAX_TIPO 20

typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int quantidade;
} Item;

// --- Prototipos ---
void limparEntrada(void);
void trimNovaLinha(char *s);
void lerLinha(const char *prompt, char *buffer, size_t tam);
int  lerInteiro(const char *prompt, int *dest);

int  iguaisIgnoreCase(const char *a, const char *b);

int  inserirItem(Item mochila[], int *tamanho);     // retorna 1 se inseriu/somou, 0 se não (capacidade cheia)
int  removerItem(Item mochila[], int *tamanho);     // retorna 1 se removeu, 0 se não encontrado
void listarItens(const Item mochila[], int tamanho);
int  buscarItem(const Item mochila[], int tamanho); // retorna índice encontrado ou -1 (também exibe os dados)

// --- Utilitários de entrada/strings ---

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
        // Em caso raro de erro de leitura, garante string vazia
        buffer[0] = '\0';
        return;
    }
    trimNovaLinha(buffer);
}

int lerInteiro(const char *prompt, int *dest) {
    printf("%s", prompt);
    int ok = scanf("%d", dest);
    limparEntrada(); // limpa o \n pendente
    return ok == 1;
}

// Comparação case-insensitive portátil
int iguaisIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

// --- Núcleo do inventário ---

// Regras de inserção:
// - Se existir item com MESMO nome (ignorando maiúsc/minúsc), soma quantidade
// - Caso contrário, insere novo até atingir CAPACIDADE
int inserirItem(Item mochila[], int *tamanho) {
    if (*tamanho >= CAPACIDADE) {
        printf("[!] Mochila cheia (capacidade %d). Remova algo antes de inserir.\n", CAPACIDADE);
        return 0;
    }

    Item novo;
    lerLinha("Nome do item (ex: Pistola, Bandagem): ", novo.nome, sizeof(novo.nome));
    while (strlen(novo.nome) == 0) {
        printf("Nome não pode ser vazio.\n");
        lerLinha("Nome do item: ", novo.nome, sizeof(novo.nome));
    }

    lerLinha("Tipo (ex: arma, municao, cura, ferramenta): ", novo.tipo, sizeof(novo.tipo));
    if (strlen(novo.tipo) == 0) strcpy(novo.tipo, "indefinido");

    while (!lerInteiro("Quantidade (>= 1): ", &novo.quantidade) || novo.quantidade < 1) {
        printf("Quantidade inválida. Tente novamente.\n");
    }

    // Verifica se já existe item com mesmo nome -> soma quantidade
    for (int i = 0; i < *tamanho; i++) {
        if (iguaisIgnoreCase(mochila[i].nome, novo.nome)) {
            mochila[i].quantidade += novo.quantidade;
            printf("[OK] '%s' já existia. Quantidade somada. Agora possui %d unidade(s).\n",
                   mochila[i].nome, mochila[i].quantidade);
            return 1;
        }
    }

    // Insere ao final
    mochila[*tamanho] = novo;
    (*tamanho)++;
    printf("[OK] Item '%s' (%s) x%d adicionado à mochila.\n", novo.nome, novo.tipo, novo.quantidade);
    return 1;
}

// Remoção por nome (case-insensitive). Compacta o vetor preservando ordem.
int removerItem(Item mochila[], int *tamanho) {
    if (*tamanho == 0) {
        printf("[!] Mochila vazia. Nada para remover.\n");
        return 0;
    }

    char alvo[MAX_NOME];
    lerLinha("Digite o nome do item a remover: ", alvo, sizeof(alvo));

    for (int i = 0; i < *tamanho; i++) {
        if (iguaisIgnoreCase(mochila[i].nome, alvo)) {
            // Desloca à esquerda
            for (int j = i; j < *tamanho - 1; j++) {
                mochila[j] = mochila[j + 1];
            }
            (*tamanho)--;
            printf("[OK] Item '%s' removido.\n", alvo);
            return 1;
        }
    }

    printf("[!] Item '%s' não encontrado.\n", alvo);
    return 0;
}

// Lista todos os itens com seus dados. Chamada após cada operação.
void listarItens(const Item mochila[], int tamanho) {
    printf("\n=========== INVENTÁRIO ===========\n");
    if (tamanho == 0) {
        printf("(vazio)\n");
    } else {
        for (int i = 0; i < tamanho; i++) {
            printf("%2d) Nome: %-28s | Tipo: %-12s | Qtd: %d\n",
                   i + 1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
        }
    }
    printf("Capacidade: %d/%d\n", tamanho, CAPACIDADE);
    printf("==================================\n\n");
}

// Busca sequencial por nome (case-insensitive). Exibe dados e retorna índice, ou -1.
int buscarItem(const Item mochila[], int tamanho) {
    if (tamanho == 0) {
        printf("[!] Mochila vazia. Nada para buscar.\n");
        return -1;
    }

    char alvo[MAX_NOME];
    lerLinha("Digite o nome do item para buscar: ", alvo, sizeof(alvo));

    for (int i = 0; i < tamanho; i++) {
        if (iguaisIgnoreCase(mochila[i].nome, alvo)) {
            printf("[ENCONTRADO] Nome: %s | Tipo: %s | Quantidade: %d\n",
                   mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
            return i;
        }
    }

    printf("[!] Item '%s' não encontrado.\n", alvo);
    return -1;
}

// --- Menu/Fluxo principal ---

void mostrarMenu(void) {
    printf("======== MOCHILA DO JOGADOR ========\n");
    printf("1 - Inserir item\n");
    printf("2 - Remover item\n");
    printf("3 - Listar itens\n");
    printf("4 - Buscar item (busca sequencial)\n");
    printf("0 - Sair\n");
    printf("====================================\n");
}

int main(void) {
    Item mochila[CAPACIDADE];
    int tamanho = 0;

    printf("Bem-vindo ao sistema de inventário!\n");
    printf("Dica: cadastre rapidamente armas, munições, kits médicos e ferramentas.\n\n");

    int opcao;
    do {
        mostrarMenu();
        if (!lerInteiro("Escolha uma opção: ", &opcao)) {
            printf("Entrada inválida. Tente de novo.\n");
            continue;
        }

        switch (opcao) {
            case 1:
                inserirItem(mochila, &tamanho);
                listarItens(mochila, tamanho);
                break;
            case 2:
                removerItem(mochila, &tamanho);
                listarItens(mochila, tamanho);
                break;
            case 3:
                listarItens(mochila, tamanho);
                break;
            case 4:
                buscarItem(mochila, tamanho);
                // Também mostramos o inventário para feedback rápido:
                listarItens(mochila, tamanho);
                break;
            case 0:
                printf("Saindo... Boas partidas!\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}
