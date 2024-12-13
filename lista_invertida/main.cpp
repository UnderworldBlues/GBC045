#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

typedef struct IndiceSecundario {
    char palavra[100];
    int offset;
} indiceSecundario;

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
    FILE *idx1; // indice primario
    FILE *idx2; // indice secundario
    // construtor
    listaInvertida() 
    { 
        idx1 = fopen("indicePrimario.bin","wb+");
        idx2 = fopen("indiceSecundario.bin","wb+");
    }
    // destrutor
    ~listaInvertida() 
    { 
        fclose(idx1);
        fclose(idx2);
        // limpa o mapa
        index.clear();
    }
    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        string pStr(palavra); // faz uma copia da palavra em um objt string
        index[pStr].push_back(offset); // adiciona a palavra no vetor de offsets
        // copia o conteudo pra estrutura de indice secundario
        indiceSecundario idx;
        strcpy(idx.palavra,palavra);
        idx.offset = offset;
        // adiciona a palavra ao indice secundario
        fwrite(&idx,sizeof(indiceSecundario),1,idx2);
        // adiciona o offset ao indice primario
        fwrite(&offset,sizeof(int),1,idx1);
    }
    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        *quantidade = 0;
        string pStr(palavra); // faz uma copia da palavra em um objt string
        auto it = index.find(pStr); // busca a palavra no indice
        if (it != index.end()) 
        { 
            // palavra encontrou
            *quantidade = it->second.size(); // pega a quantidade de offsets
            int *offsets = new int[*quantidade]; // aloca vetor de offsets
            for (int i = 0; i < *quantidade; i++) // copia os offsets
                offsets[i] = it->second[i];
            return offsets; // retorna o vetor
        }
        else
            return NULL; // nao encontrada     
    }

    // p conferir os arquivos de indice
    void showIndex() {
        fseek(idx1,0,SEEK_SET);
        fseek(idx2,0,SEEK_SET);
        int offset, contador = 0;
        indiceSecundario idx;
        while (fread(&offset,sizeof(int),1,idx1) == 1) {
            printf("%d ",offset);
            contador++;
            if (contador % 10 == 0) break;
        }
        printf("\n");
        contador = 0;
        while (fread(&idx,sizeof(indiceSecundario),1,idx2) == 1) {
            printf("%s %d\n",idx.palavra,idx.offset);
            contador++;
            if (contador % 10 == 0) break;
        }
    }

private:
    // esse mapa armazena uma lista de offsets para cada palavra unica
    map<string, vector<int>> index; 
};

// programa principal
int main(int argc, char** argv) {
    // abrir arquivo
    ifstream in("biblia.txt");
    if (!in.is_open()){
        printf("\n\n Nao consegui abrir arquivo biblia.txt. Sinto muito.\n\n\n\n"); // nao precisa se desculpar bertao
    }
    else
    {
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
        // lista.showIndex();

        // agora que ja construimos o indice, podemos realizar buscas
        do {
            printf("\nDigite a palavra desejada ou \"SAIR\" para sair: ");
            scanf("%s",palavra);
            if (strcmp(palavra,"SAIR") != 0) {
                int quantidade;
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
        lista.~listaInvertida();
    }

    return (EXIT_SUCCESS);
}
