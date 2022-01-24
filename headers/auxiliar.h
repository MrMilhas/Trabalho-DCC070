#ifndef AUXILIAR_h
#define AUXILIAR_h

#include <iostream>
#include <string.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdio.h>

using namespace std;

/**
 * @brief       Função auxiliar para converter binário em decimal.
 * 
 * @param bin   String que armazena o valor em binário.
 * @return int  Retorna o valor convertido em inteiro.
 */
int converteBin(string bin){
    int tam = strlen(bin.c_str());  // Tamanho da string bin;
    int i = 0;                      // Inteiro de controle para conversão;
    int dec = 0;                    // Decimal que será obtido.

    // Percorre a string realizando as potências de 2;
    while(tam>0){
        if(bin[tam] == '0' || bin[tam] == '1'){
            dec = dec + pow(2, i++) * (bin[tam] - '0');
        }
    }

    return dec;  // Retorna decimal;
}

/**
 * @brief        Função auxiliar para ler o arquivo com os bits de informações.
 * 
 * @param arq    Arquivo que será lido.
 * @return int*  Retorna o vetor memória com todos os valores armazenados.
 */
int *lerArquivo(fstream &arq){
    string aux;    // String auxiliar para captura da linha;
    int *memoria;  // Vetor memória para armazenar os valores lidos;
    int i = 0;     // Inteiro para controle de índices do vetor;

    arq.seekg(0);  // Posicionando ponteiro no início do arquivo;

    // Enquanto houver linhas, captura essas linhas;
    while(arq.good()){
        getline(arq, aux);              // Captura a linha e armazena em aux;

        memoria[i] = converteBin(aux);  // Armazena o valor convertido no vetor memória;
        i++;
    }
    return memoria;  // Retorna o vetor memória;
}

#endif