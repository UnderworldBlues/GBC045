
// Alunos: Pedro Souza Ferreira (12211BCC023) & Marcel Fernando Lobo de Feo (12211BCC042)
 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct indiceSecundario {
    string proverbio;
    int indice;
};

// remove pontuacao de uma palavra
void removePontuacao (char *palavra) {
    int length = strlen(palavra);
    if (
        (palavra[length-1] == '.') || (palavra[length-1] == ',') || (palavra[length-1] == ';') ||
        (palavra[length-1] == ':') || (palavra[length-1] == '?') || (palavra[length-1] == '!')
       )
        palavra[length-1] = '\0';
}

// imprime linha do arquivo com base no offset da palavra
void imprimeLinha(int offset,FILE *f) {
    int pos = ftell(f);
    char linha[2048];
    while (pos < offset) {
        fgets(linha,2047,f);
        pos = ftell(f);
    }
    printf("%s",linha);
}

// classe que implementa a lista invertida
class listaInvertida {
public:
    FILE *f;// = fopen("listaInvertida.bin","wb+");
    FILE *g;// = fopen("indiceSecundario.bin","wb+");

    // construtor
    listaInvertida() {
        f = fopen("listaInvertida.bin","wb+");
        g = fopen("indiceSecundario.bin","wb+");
    }
    // destrutor
    ~listaInvertida() { 
        // ordenar
        ordenaIndice();
        // guardar no arquivo
        fseek(f,0,SEEK_END);
        int tamanho = ftell(f)/sizeof(int);
        fseek(f,0,SEEK_SET);
        int *offsets = new int[tamanho];
        fread(offsets,sizeof(int),tamanho,f);

        fclose(g);
        fclose(f);
    }
    // ordena indices secundarios
    void ordenaIndice() {
        fseek(f,0,SEEK_END);
        int tamanho = ftell(f)/sizeof(struct indiceSecundario);
        fseek(f,0,SEEK_SET);
        struct indiceSecundario *indices = new struct indiceSecundario[tamanho];
        fread(indices,sizeof(struct indiceSecundario),tamanho,f);
    }
    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        fwrite(&offset,sizeof(int),1,f);
        // adicionar palavra no indice secundario
        struct indiceSecundario ind;
        ind.proverbio = palavra;
        ind.indice = offset;
        fwrite(&ind,sizeof(struct indiceSecundario),1,g);

    }
    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        // substituir pelo resultado da busca na lista invertida
        int *offsets = new int[10];
        struct indiceSecundario ind;

        while (fread(&ind,sizeof(struct indiceSecundario),1,g) > 0) 
        {
            if (strcmp(ind.proverbio.c_str(),palavra) == 0) 
            {
                offsets[*quantidade] = ind.indice;
                *quantidade++;
                if (*quantidade == 10) break; // tem q tirar isso aqui dps
            }
        }
        return offsets;
    }
private:
};

// programa principal
int main(int argc, char** argv) {
    // abrir arquivo
    ifstream in("biblia.txt");
    if (!in.is_open()){
        printf("\n\n Nao consegui abrir arquivo biblia.txt. Sinto muito.\n\n\n\n"); // nao precisa pedir desculpas bertao
    }
    else{
        // vamos ler o arquivo e criar a lista invertida com as palavras do arquivo
        char *palavra = new char[100];
        int offset, contadorDePalavras = 0;
        listaInvertida lista;
        // ler palavras
        while (!in.eof()) {
            // ler palavra
            in >> palavra;
            // pegar offset
            offset = in.tellg();
            // remover pontuacao
            removePontuacao(palavra);
            // desconsiderar palavras que sao marcadores do arquivo
            if (!((palavra[0] == '#') || (palavra[0] == '[') || ((palavra[0] >= '0') && (palavra[0] <= '9')))) {
                //printf("%d %s\n", offset,palavra); fflush(stdout); // debug :-)
                lista.adiciona(palavra, offset);
                contadorDePalavras++;
                if (contadorDePalavras % 1000 == 0) { printf(".");  fflush(stdout); }
            }
        }
        in.close();

        // agora que ja construimos o indice, podemos realizar buscas
        do {
            printf("\nDigite a palavra desejada ou \"SAIR\" para sair: ");
            scanf("%s",palavra);
            if (strcmp(palavra,"SAIR") != 0) {
                int quantidade = 0;
                // busca na lista invertida
                int *offsets = lista.busca(palavra,&quantidade);
                // com vetor de offsets, recuperar as linhas que contem a palavra desejada
                if (quantidade > 0) {
                    FILE *f = fopen("biblia.txt","rt");
                    for (int i = 0; i < quantidade; i++)
                        imprimeLinha(offsets[i],f);
                    fclose(f);
                }
                else
                    printf("nao encontrou %s\n",palavra);
            }
        } while (strcmp(palavra,"SAIR") != 0);

        printf("\n\nAte mais!\n\n");
    }

    return (EXIT_SUCCESS);
}

