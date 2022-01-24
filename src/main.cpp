#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>

using namespace std;
/**
 * @brief         Função para verificar a existência do arquivo contendo as instruções.
 * 
 * @param dir     Diretório do arquivo.
 * @return true   Retorna true se o arquivo existir.
 * @return false  Retorna false se o arquivo não existir.
 */
bool verificaDir(string dir){
    cout << "Iniciando verificação do diretório:" << endl;
    cout << endl;

    string dirArq = dir + "/instrucoes.txt";

    ifstream arq(dirArq, ios::in);

    if(!arq.is_open()){
        cout << "O arquivo 'instrucoes.txt' ainda não existe." << endl;
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
        cout << "Digite uma das opções: " << endl;
        cout << "   1 - Leitura dos bits de informação do arquivo." << endl;
        cout << "   2 - Passagem dos bits de informação por teclado." << endl;
        cout << "   0 - Para sair do programa." << endl;
        cout << endl;

        int escolha;

        cout << "Escolha: " << endl;
        cin >> escolha;
        cout << "_____________________________________________________________________________" << endl;
        cout << endl;

        if(escolha != 1 && escolha != 2 && escolha != 0){
            cout << "Digite um valor válido." << endl;
            cout << endl;
        }
        else{
            if(escolha == 1){
                string dir;
                cout << "Digite o diretório do arquivo contendo as informações: " << endl;
                cin >> dir;

                if(!verificaDir(dir)){
                    cout << "Iniciando criação automática do arquivo: " << endl;
                }
                else{
                    // Chamar função para execução das instruções.
                }
            }

            if(escolha == 2){
                string bits;
                cout << "Digite os bits de informação para a execução da intrução: " << endl;
                cin >> bits;
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

}