#include <iostream>
#include <fstream>
#include <math.h>
#include <bitset>
#include <iomanip>
#include "../headers/auxiliar.h"
#include "../headers/pipeline.h"

using namespace std;

/**
 * @brief         Função para verificar a existência do arquivo contendo as instruções.
 * 
 * @param dir     Diretório do arquivo.
 * @return true   Retorna true se o arquivo existir.
 * @return false  Retorna false se o arquivo não existir.
 */
bool verificaDir(string dir){
    cout << "Iniciando verificacao do diretorio:" << endl;
    cout << endl;

    string dirArq = dir + "/instrucoes.txt";

    ifstream arq(dirArq, ios::in);

    if(!arq.is_open()){
        cout << "O arquivo 'instrucoes.txt' ainda nao existe." << endl;
        cout << endl;
        return 0;
    }
    else{
        cout << "Arquivo 'instrucoes.txt' encontrado. " << endl;
        cout << endl;
        return 1;
    }
}

/**
 * @brief  Menu.
 * 
 */
void menu(){
    bool continua = true;

    while(continua){
        cout << "Digite uma das opcoes: " << endl;
        cout << "   1 - Leitura dos bits de informacao do arquivo." << endl;
        cout << "   2 - Passagem dos bits de informacao por teclado." << endl;
        cout << "   0 - Para sair do programa." << endl;
        cout << endl;

        int escolha;
        int escolha2;

        cout << "Escolha: " << endl;
        cin >> escolha;
        cout << "_____________________________________________________________________________" << endl;
        cout << endl;

        if(escolha != 1 && escolha != 2 && escolha != 0){
            cout << "Digite um valor valido." << endl;
            cout << endl;
        }
        else{
            
            // Inicializando vetor de memoria;
            for(int i=0; i<512; i++){
                memoria[i] = bitset<32>(1);
            }

            // Inicializando veotr de registradores;
            for(int i=0; i<32; i++){
                registradores[i] = bitset<32>(1);
            }

            if(escolha == 1){
                string dir;
                cout << "Digite o diretorio do arquivo contendo as informacoes: " << endl;
                cin >> dir;

                if(!verificaDir(dir)){
                    cout << "ERRO: Diretorio nao encontrado." << endl;
                    continua = false;
                }
                else{
                    cout << "Iniciando leitura do arquivo: " << endl;
                    fstream arq(dir+"/instrucoes.txt", ios::in);
                    lerArquivo(arq, memoria);
                    cout << "Arquivo lido com sucesso. Memoria de instruções atualizada." << endl;
                    cout << "___________________________________________________________" << endl;
                    arq.close();
                    
                    cout << "Escolha o modo de execucaoo: " << endl;
                    cout << "       1 - Seriada." << endl;
                    cout << "       2 - Direta." << endl;
                    cout << endl;
                    cout<< "Escolha: " << endl;
                    cin >> escolha2;
                    cout << endl;

                    pipeline();
                    
                }
            }

            if(escolha == 2){
                string bits;
                cout << "Digite as instrucoes que deseja executar: " << endl;

                int opcao = 1;
                while(opcao == 1){
                    cin >> bits;

                    cout << "Deseja digitar mais uma instrucao? " << endl;
                    cout << "   1 - Digitar." << endl;
                    cout << "   2 - Parar." << endl;
                    cout << "Opção: " << endl;
                    cin >> opcao;
                    cout << endl; 
                }

                cout << "Valores lidos com sucesso. Memoria de instrucoes atualizada." << endl;
                cout << "___________________________________________________________" << endl;

                cout << "Escolha o modo de execucao: " << endl;
                    cout << "       1 - Seriada." << endl;
                    cout << "       2 - Direta." << endl;
                    cout << endl;
                    cout<< "Escolha: " << endl;
                    cin >> escolha2;
                    cout << endl;

                pipeline();
            }

            if(escolha == 0){
                continua = false;
                cout << "Finalizando programa." << endl;
                cout << "_____________________________________________________________________________" << endl;
            }
        }
    }
}

/**
 * @brief Main.
 * 
 * @return int 0.
 */
int main(){
    
   menu();
}