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

void criptografar(){ // Função que criptografa uma mensagem por um deslocamento de escolha do usuário

    int key, indice_final, indice;

    char mensagem[1000];
    char cifrado[1000];

    banner();
    printf("Digite sua frase: ");
    getchar();                              // Coleta da Mensagem que será encriptada
    fgets(mensagem, sizeof(mensagem), stdin);
    printf("Qual a chave de deslocamento: ");
    scanf("%d", &key);                      // Coleta da Chave de Deslocamento

    //Inicio da encriptação
    for (indice = 0; mensagem[indice] != '\0'; indice++) {
        if (mensagem[indice] == ' ') { //Análise de espaços em branco
            cifrado[indice] = ' ';
        } else if (isalpha(mensagem[indice])) {
            char indice_referencia = isupper(mensagem[indice]) ? 'A' : 'a'; //Análise de letras maiúsculas ou minúsculas
            cifrado[indice] = (mensagem[indice] - indice_referencia + key) % 26 + indice_referencia;
        } else {
            cifrado[indice] = mensagem[indice]; // mantém pontuação, acento, etc.
        }
    }
    cifrado[indice] = '\0';
    system("cls");
    banner();
    printf("Chave de Deslocamento: %d\n", key);
    printf("Texto original: %s", mensagem);
    printf("Texto cifrado : %s\n", cifrado);

}

void descriptografar(){ // Função que descriptografa uma mensagem por força bruta e pela tabela de frequência

    int indice;

    char mensagem[1000];
    char tentativa[1000];

    printf("--------------------------------------\n");
    printf("-           DESCRIPTOGRAFIA          -\n");
    printf("--------------------------------------\n");
    printf("Digite sua frase cifrada: ");
    getchar();
    fgets(mensagem, sizeof(mensagem), stdin);

    printf("\n>>> ATAQUE POR FORÇA BRUTA:\n");  //Ataque por força bruta testa todas as possíveis chaves
    for (int key = 1; key < 26; key++) {
        for (indice = 0; mensagem[indice] != '\0'; indice++) {
            if (mensagem[indice] == ' ') {
                tentativa[indice] = ' ';
            } else if (isalpha(mensagem[indice])) {
                char ref = isupper(mensagem[indice]) ? 'A' : 'a';
                tentativa[indice] = (mensagem[indice] - ref - key + 26) % 26 + ref;
            } else {
                tentativa[indice] = mensagem[indice];
            }
        }
        tentativa[indice] = '\0';
        printf("k=%2d | %s", key, tentativa);
    }


    int cont_letras, letra_idx;   // --- ATAQUE POR DISTRIBUIÇÃO DE FREQUÊNCIA (com freqPT) ---
    double melhor_erro = 1e9;
    int melhor_k = -1;
    char tentativa_freq[1000];

    for (int k = 1; k < 26; k++){   // Testa todas as chaves possíveis

        int contador[26] = {0};
        cont_letras = 0;

        for (indice = 0; mensagem[indice] != '\0'; indice++) {   // Decifra com chave k e conta letras

            if (isalpha(mensagem[indice])) {

                char indice_referencia = isupper(mensagem[indice]) ? 'A' : 'a';
                char decif = (mensagem[indice] - indice_referencia - k + 26) % 26 + indice_referencia;

                tentativa_freq[indice] = decif;
                letra_idx = tolower(decif) - 'a';
                contador[letra_idx]++;
                cont_letras++;

            } else {
                tentativa_freq[indice] = mensagem[indice];
            }
        }
        tentativa_freq[indice] = '\0';


        double erro = 0.0;             // Calcula distribuição e erro
        for (int i = 0; i < 26; i++) {
            double freq = ((double)contador[i] / cont_letras) * 100.0;
            erro += fabs(freq - freqPT[i]);
        }

        if (erro < melhor_erro) { // Atualiza melhor k
            melhor_erro = erro;
            melhor_k = k;
            strcpy(tentativa, tentativa_freq);
        }
    }
    printf("\n>>> ATAQUE POR ANÁLISE DE FREQUÊNCIA (COMPARAÇÃO DE DISTRIBUIÇÃO):\n");
    printf("Chave estimada: %d\n", melhor_k);
    printf("Erro médio: %.2f\n", melhor_erro);
    printf("Mensagem provável: %s\n", tentativa);
}

int main(){
    setlocale(LC_ALL,"Portuguese_Brazil");

    int opcao;

    do{
        opcao = menu();
        switch(opcao){
            case 1:
                    system("cls");
                    criptografar();
                break;
            case 2:
                    system("cls");
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