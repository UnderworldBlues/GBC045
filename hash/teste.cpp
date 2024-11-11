// Pedro Souza Ferreira - 12211BCC023
// Marcel Feo - 12211BCC042
/* 
O problema proposto pedia a implementação de uma função de hash eficiente, com boa distribuição e baixo número de colisões. 
Para isso, foram implementadas duas funções de hash, a primeira foi a recomendada pelo professor, do livro "Sistemas de Banco de Dados", implementada em getHash.
A segunda foi a função de hash polinomial, "polynomial rolling hash", baseada em https://cp-algorithms.com/string/string-hashing.html.
Comparando as duas, a função de hash polinomial é a mais eficiente, gerando em torno de ~30% menos colisões.
Por exemplo; a função de hash recomendada, quando aplicada sobre o arquivo 'cidadesbr.txt' (5570 entradas) em buckets de 50KB, gera 389 colisões no total.
Enquanto a função polinomial gera 298 colisões.  
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;
// buckets para cidadesbr.txt
#define H30KB 30720 // 30KB
#define H40KB 40960 // 40KB
#define H50KB 51200 // 50KB
// buckets para dblp.txt
#define H300KB 307200 // 300KB
#define H400KB 409600 // 400KB
#define H500KB 512000 // 500KB

// estrutura de hash polinomial
struct Hash {
    long long p = 31, m = 1e9 + 7;
    long long hash_value;

    Hash(const std::string& s) 
    {
        long long hashAtual = 0;
        long long p_pow = 1;
        const long long n = s.length();
        for (long long i = 0; i < n; ++i) 
        {
            hashAtual = (hashAtual + (s[i] - 'a' + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
        // pra garantir valores positivos
        hash_value = (hashAtual + m) % m;
    }

    bool operator==(const Hash& other) 
    {
        return (hash_value == other.hash_value);
    }
};

int getHash(char *str, int Hsize);

int main()
{
        // abre o arquivo
    FILE *f = fopen("cidadesbr.txt","rb");
    if (f == NULL) 
    {
        printf("Sinto muito, nao consegui abrir arquivo de dados.");
        return 0;
    }

        // define o tamanho do histograma
    int HSIZE = H50KB; 
    // conta o numero de colisoes
    int overflows = 0;
    // vetor que armazenara histograma de contagem
    long histograma[HSIZE];
    // preenche o vetor
    for (int i = 0; i < HSIZE; i++)
        histograma[i] = 0;

    printf("working on it...\n");
    while (true) 
    {
        // ler linha
        char linhaArq[1024];
        string buffer;
        fgets(linhaArq,1024,f);
        buffer = linhaArq;
        if (feof(f))
            break;
        // calcula o hash
        Hash h(buffer);
        // verifica se houve colisao
        if(histograma[h.hash_value % HSIZE] != 0)
            overflows++;
        // incrementa o histograma
        histograma[h.hash_value % HSIZE]++;
        
    }

    fclose(f);

    int media = 0;
    for(int i = 0; i < HSIZE; i++)
        media += histograma[i];

    media = media / HSIZE;

    printf("Tamanho da tabela hash: %d\nNumero total de colisoes: %d\nMedia de colisoes por bucket: %d", HSIZE, overflows, media);

    // criar grafico de contagem do hash
    FILE *grafico = fopen("grafico.plt","wb");
    fprintf(grafico,"set title \"Histograma\"\n");
    fprintf(grafico,"set encoding iso_8859_1\n");
    fprintf(grafico,"set xlabel \"Hash\"\n");
    fprintf(grafico,"set ylabel \"Quantidade\"\n");
    fprintf(grafico,"plot '-' title 'Quantidade' with linespoints linewidth 2 linetype 1 pointtype 1\n");
    for (int i = 0; i < HSIZE; i++)
        fprintf(grafico,"%d %d\n",i,histograma[i]);
    fprintf(grafico,"end\n");
    fprintf(grafico,"pause -1\n");
    fclose(grafico);

	return 0;
}

// função recomendada pelo professor
int getHash(char *str, int Hsize)
{
    int len = strlen(str);
    int hash = 0;

    for (int i = 0; i < (len - 1); i++)
        hash += str[i] * (47 ^ (i - 1)); // 47 como chave de hash
    hash = hash % Hsize;
    
    return hash;
}