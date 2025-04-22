#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>

double freqPT[26] = { //Tabela de frequência de cada letra no português
    13.9, 1.0, 4.4, 5.4, 12.2,
    1.0 , 1.2, 0.8, 6.9, 0.4 ,
    0.1 , 2.8, 4.2, 5.3, 10.8,
    2.9 , 0.9, 6.9, 7.9, 4.9 ,
    4.0 , 1.3, 0.0, 0.3, 0.0 , 0.4 };

void banner(){ //Banner do programa
    printf("--------------------------------------\n");
    printf("---          CRIPTOGRAFIA          ---\n");
    printf("--------------------------------------\n");
}

int menu(){ // Menu de opções
    int opcao;
    banner();
    printf("1. CRIPTOGRAFAR \n");
    printf("2. DESCRIPTOGRAFAR \n");
    printf("3. SAIR \n");
    printf("DIGITE SUA OPCAO: ");
    scanf("%d",&opcao);
    return opcao;
}

void criptografar() {
    int ordem[4];
    char texto[200];

    printf("Digite o texto a ser criptografado (sem acentos):\n");
    getchar(); // limpar buffer do scanf anterior
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = '\0'; // remover o \n

    // Entrada da permutação
    printf("Informe a ordem de permutação das colunas (4 números distintos entre 0 e 3):\n");
    for (int i = 0; i < 4; i++) {
        printf("Posição %d: ", i + 1);
        scanf("%d", &ordem[i]);
    }

    // Validação simples da permutação
    int usado[4] = {0};
    for (int i = 0; i < 4; i++) {
        if (ordem[i] < 0 || ordem[i] > 3 || usado[ordem[i]]) {
            printf("Permutação inválida! Cada valor deve ser único e entre 0 e 3.\n");
            return;
        }
        usado[ordem[i]] = 1;
    }

    int tamanho = strlen(texto);
    int colunas = 4;
    int linhas = (tamanho + colunas - 1) / colunas;

    char **matriz = malloc(linhas * sizeof(char *));
    for (int i = 0; i < linhas; i++)
        matriz[i] = malloc(colunas * sizeof(char));

    int k = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (k < tamanho)
                matriz[i][j] = texto[k++];
            else
                matriz[i][j] = 'X';
        }
    }

    printf("\nMatriz preenchida (%dx%d):\n", linhas, colunas);
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%c ", matriz[i][j]);
        }
        printf("\n");
    }

    printf("\nTexto criptografado:\n");
    for (int i = 0; i < colunas; i++) {
        int col = ordem[i];
        for (int lin = 0; lin < linhas; lin++) {
            printf("%c", matriz[lin][col]);
        }
    }
    printf("\n");

    for (int i = 0; i < linhas; i++) free(matriz[i]);
    free(matriz);
}

void descriptografar() {
    char cifra[200];
    printf("Digite o texto criptografado:\n");
    getchar(); // limpar o buffer
    fgets(cifra, sizeof(cifra), stdin);
    cifra[strcspn(cifra, "\n")] = '\0';

    int tamanho = strlen(cifra);
    int colunas = 4;
    int linhas = tamanho / colunas;

    int perm[24][4] = {
        {0,1,2,3},{0,1,3,2},{0,2,1,3},{0,2,3,1},{0,3,1,2},{0,3,2,1},
        {1,0,2,3},{1,0,3,2},{1,2,0,3},{1,2,3,0},{1,3,0,2},{1,3,2,0},
        {2,0,1,3},{2,0,3,1},{2,1,0,3},{2,1,3,0},{2,3,0,1},{2,3,1,0},
        {3,0,1,2},{3,0,2,1},{3,1,0,2},{3,1,2,0},{3,2,0,1},{3,2,1,0}
    };

    char **matriz = malloc(linhas * sizeof(char *));
    for (int i = 0; i < linhas; i++)
        matriz[i] = malloc(colunas * sizeof(char));

    double menorDistancia = 1e9;
    char melhorTentativa[200];
    int melhorPerm[4];

    printf("\n--- FORÇA BRUTA COM ANÁLISE DE FREQUÊNCIA ---\n");

    for (int p = 0; p < 24; p++) {
        int ordem[4];
        for (int i = 0; i < 4; i++) ordem[i] = perm[p][i];

        int k = 0;
        for (int i = 0; i < 4; i++) {
            int col = ordem[i];
            for (int lin = 0; lin < linhas; lin++) {
                matriz[lin][col] = cifra[k++];
            }
        }

        char resultado[200];
        int idx = 0;
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                resultado[idx++] = matriz[i][j];
            }
        }
        resultado[idx] = '\0';

        int cont[26] = {0}, letrasValidas = 0, limite = idx;
        if (resultado[limite - 1] == 'X') limite--;
        if (resultado[limite - 1] == 'X') limite--;

        for (int i = 0; i < limite; i++) {
            char ch = tolower(resultado[i]);
            if (ch >= 'a' && ch <= 'z') {
                cont[ch - 'a']++;
                letrasValidas++;
            }
        }

        double freqTentativa[26];
        for (int i = 0; i < 26; i++)
            freqTentativa[i] = (letrasValidas > 0) ? (cont[i] * 100.0 / letrasValidas) : 0.0;

        double dist = 0;
        for (int i = 0; i < 26; i++) {
            double diff = freqTentativa[i] - freqPT[i];
            dist += diff * diff;
        }
        dist = sqrt(dist);

        printf("Permutação %2d (%d%d%d%d): %s [distância=%.2f]\n", p+1,
               ordem[0], ordem[1], ordem[2], ordem[3], resultado, dist);

        if (dist < menorDistancia) {
            menorDistancia = dist;
            strcpy(melhorTentativa, resultado);
            for (int i = 0; i < 4; i++) melhorPerm[i] = ordem[i];
        }
    }

    printf("\n--- MELHOR TENTATIVA PELA FREQUÊNCIA ---\n");
    printf("Ordem: %d%d%d%d\n", melhorPerm[0], melhorPerm[1], melhorPerm[2], melhorPerm[3]);
    printf("Texto mais próximo do português:\n%s\n", melhorTentativa);

    for (int i = 0; i < linhas; i++) free(matriz[i]);
    free(matriz);
}

int main(){

    setlocale(LC_ALL,"Portuguese_Brazil");

    int opcao;

    do{
        opcao = menu();
        switch(opcao){
            case 1:
                //   system("cls");
                    criptografar();
                break;
            case 2:
                //    system("cls");
                    descriptografar();

                break;
            case 3:
                    printf("\n");
                    printf("Saindo...\n");
                    printf("\n");
                    break;
            }
    }while (opcao != 3);
    return 0;
}