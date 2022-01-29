#include <iostream>
#include <fstream>
#include <math.h>
#include <bitset>
#include <iomanip>
using namespace std;

/**
 * @brief Etapa 1 do Pipeline, Busca da Instrucao
 * 
 */
struct IFStruct {
    bitset<32>  PC;
    bool        nop;  //para tratamento dos Hazards indica se esta realizando uma operacao valida no ciclo de clock 
};

/**
 * @brief Etapa 2 do Pipeline, Decodificacao da Instrucao e leitura dos registradores
 * 
 */
struct IDStruct {
    bitset<32>  instrucao; //para armazenar a instrucao do array memoria
    bool        nop;  //para tratamento dos Hazards indica se esta realizando uma operacao valida no ciclo de clock 
};

/**
 * @brief Etapa 3 do Pipeline, Execucao e calculo do endereco
 * 
 */
struct EXStruct {

    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  immediate; //campo para armazenar o immediate das operacoes do tipo I
    bitset<5>   rs; //registrador de origem 
    bitset<5>   rt; //registrador de origem, e registrador destino para operacoes do tipo I
    bitset<5>   Wrt_reg_addr; //endereco de memoria onde ele vai armazenar (indice do vetor de registradores)
    bool        is_I_type;
    bool        rd_mem;//possivelmente eh o MemRead, verificar
    bool        wrt_mem; //possivelmente eh o MemWrite, verificar
    bool        alu_op;  //verificar se esse sinal de controle ira ficar aqui mesmo  
    bool        wrt_enable; //possivelmente eh o RegWrite, verificar
    bool        nop; //para tratamento dos Hazards indica se esta realizando uma operacao valida no ciclo de clock  
    
    string      INS; //para colocar qual tipo de instrucao
};

/**
 * @brief Etapa 4 do Pipeline, Acesso a memoria de Dados
 * 
 */
struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   rs; //registrador de origem 
    bitset<5>   rt;  //registrador de origem, e registrador destino para operacoes do tipo I   
    bitset<5>   Wrt_reg_addr; //endereco de memoria onde ele vai armazenar (indice do vetor de registradores)
    bool        rd_mem; //possivelmente eh o MemRead, verificar
    bool        wrt_mem; //possivelmente eh o MemWrite, verificar
    bool        wrt_enable; //possivelmente eh o RegWrite, verificar   
    bool        nop; //para tratamento dos Hazards indica se esta realizando uma operacao valida no ciclo de clock    
    
    string      INS; //para colocar qual tipo de instrucao
};

/**
 * @brief Etapa 5 do Pipeline, Escrita do Resultado
 * 
 */

struct WBStruct {
    bitset<32>  Wrt_data; //conteudo a ser salvo 
    bitset<5>   rs; //registrador de origem 
    bitset<5>   rt; //registrador de origem, e registrador destino para operacoes do tipo I    
    bitset<5>   Wrt_reg_addr; //endereco de memoria onde ele vai armazenar (indice do vetor de registradores)
    bool        wrt_enable; //possivelmente eh o RegWrite, verificar 
    bool        nop; //para tratamento dos Hazards indica se esta realizando uma operacao valida no ciclo de clock  
    
    string      INS; //para colocar qual tipo de instrucao 
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

