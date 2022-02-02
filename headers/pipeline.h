#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include "auxiliar.h"


using namespace std;

//Variaveis Globais

bitset<32> memoria[512];      //Array que contem em cada posicao contem uma instrucao

bitset<32> registradores[32]; //Array que contem em cada posicao um registrador, o array todo corresponde ao banco de registradores 

//? Inicia no ciclo de clock 0;
int cycle;

bitset<32> extensorSinal(bitset<16> Imm){

    string se0 = "0000000000000000";
    string se1 = "1111111111111111";
    string immediate = Imm.to_string();
    bitset<32> se_imm;
    
    if (immediate.substr(0,1) == "0"){
        se0 += immediate;
        se_imm = bitset<32>(se0);
    }					
    else{
        se1.append(immediate);
        se_imm = bitset<32>(se1);				
    }
    return se_imm;
}

//? ------------------------------------------------------------------------------------------------------------------------------------
struct etapaIF {
    bitset<32>  PC;   // Registrador PC;
    bool        nop;  // Variável para tratamento dos Hazzards;
};

struct etapaID {
    bitset<32>  instrucao;  // Bitiset para armazenar a instrução;
    bool        nop;        // Variável para tratamento dos Hazzards;
};

struct etapaEX {
    bitset<32>  regA;          // Registrador A;
    bitset<32>  regB;          // Registrador B;
    bitset<16>  Immediate;     // Campo para armazenar o immediate das operações do tipo I;
    bitset<5>   rs;            // Registrador rs;
    bitset<5>   rt;            // Registrador rt;
    bitset<5>   endereco;      // Endereço de memória para auxiliar leitura e escrita;
    bool        tipo_I;        // Booleano para verificar se a intrução é do tipo I;
    bool        memRead;       // Sinal de controle MemRead;
    bool        memWrite;      // Sinal de controle MemWrite;
    bitset<2>   aluOp;         // Sinal de controle AluOp; 
    bool        regWrite;      // Sinal de controle RegWrite;
    bool        nop;           // Variável para tratamento dos Hazzards;
    string      nome;          // Nome da intrução;
};

struct etapaMEM {
    bitset<32>  resultadoALU;     // Resultado da ALU;
    bitset<32>  data_forwarding;  // Campo para forwarding;
    bitset<5>   rs;               // Registrador rs;
    bitset<5>   rt;               // Registrador rt;
    bitset<5>   endereco;         // Endereço de memória para auxiliar leitura e escrita; 
    bool        memRead;          // Sinal de controle MemRead;
    bool        memWrite;         // Sinal de controle MemWrite;
    bool        regWrite;         // Sinal de controle RegWrite;
    bool        nop;              // Variável para tratamento dos Hazzards;
    string      nome;             // Nome da intrução;
};

struct etapaWB {
    bitset<32>  info;          // Dados a serem salvos no banco de registradores;
    bitset<5>   rs;            // Registrador rs;
    bitset<5>   rt;            // Registrador rt;
    bitset<5>   endereco;      // Endereço de memória para auxiliar leitura e escrita;
    bool        regWrite;      // Sinal de controle RegWrite;
    bool        nop;           // Variável para tratamento dos Hazzards;
    string      nome;          // Nome da intrução;
};

struct ciclo {
    etapaIF    IF;   // Etapa 1;
    etapaID    ID;   // Etapa 2;
    etapaEX    EX;   // Etapa 3;
    etapaMEM   MEM;  // Etapa 4;
    etapaWB    WB;   // Etapa 5;
};

//?------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief Função utilizada para imprimir as etapas do pipeline em um arquivo de saída.
 * 
 * @param outfile     Arquivo de saída.
 * @param clockAtual  Clock para exibição das conformações.
 */
void imprimePipeLine(ofstream &outfile, ciclo clockAtual){

    outfile << "Ciclo de clock atual: " << cycle << endl;
    outfile << "Conteúdo do Banco de Registradores: " << endl;
    outfile << endl;

    for(int i = 0; i < 32; i++){
        outfile << "[" << i << "]: " << registradores[i] << endl;
    }
    outfile << endl;

    outfile << "------------- Etapa IF (Leitura da Instrução) ------------- " << endl;
    outfile << "PC: " << clockAtual.IF.PC << endl;
    outfile << endl;

    outfile << "------------- Etapa ID (Decodificação da Instrução) ------------- " << endl;
    outfile << "Instrução: " << clockAtual.ID.instrucao << endl;
    outfile << endl;

    outfile << "------------- Etapa EX (Execução/Cálculo do Endereço) ------------- " << endl;
    outfile << "Instrução: " << clockAtual.EX.nome << endl;
    outfile << "Rs: " << clockAtual.EX.rs << endl;
    outfile << "Rt: " << clockAtual.EX.rt << endl;
    outfile << "Rd: " << clockAtual.EX.endereco << endl;
    outfile << "MemRead: " << clockAtual.EX.memRead << endl;
    outfile << "MemWrite: " << clockAtual.EX.memWrite << endl;
    outfile << "Aluop: " << clockAtual.EX.aluOp << endl;
    outfile << "RegWrite: " << clockAtual.EX.regWrite << endl;
    outfile << endl;

    outfile << "------------- Etapa MEM (Acesso à Memória de Dados) ------------- " << endl;
    outfile << "Instrução: " << clockAtual.MEM.nome << endl;
    outfile << "Rs: " << clockAtual.MEM.rs << endl;
    outfile << "Rt: " << clockAtual.MEM.rt << endl;
    outfile << "Rd: " << clockAtual.MEM.endereco << endl;
    outfile << "MemRead: " << clockAtual.MEM.memRead << endl;
    outfile << "MemWrite: " << clockAtual.MEM.memWrite << endl;
    outfile << "RegWrite: " << clockAtual.MEM.regWrite << endl;
    outfile << endl;

    outfile << "------------- Etapa WB (Escrita do Resultado) ------------- " << endl;
    outfile << "Instrução: " << clockAtual.WB.nome << endl;
    outfile << "Rs: " << clockAtual.WB.rs << endl;
    outfile << "Rt: " << clockAtual.WB.rt << endl;
    outfile << "Rd: " << clockAtual.WB.endereco << endl;
    outfile << "RegWrite: " << clockAtual.WB.regWrite << endl;
    outfile << endl;

    outfile << "___________________________________________________________________________________________" << endl;
}


//?------------------------------------------------------------------------------------------------------------------------------------

void pipeline(){

    ofstream outfile ("outfile.txt");
    
    //? Inicializando etapas do pipeline com seus respectivos sinais de controle;
    etapaIF IF = {0, 0};                                              // Nop == 0 para iniciar a primeira instrução;
    
    etapaID ID = {0, 1};                                              // Nop == 1 para iniciar a primeira instrução;
    
    etapaEX EX = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, "Indefinida"};  // Nop == 1 para iniciar a primeira instrução;
    
    etapaMEM MEM = {0, 0, 0, 0, 0, 0, 0, 0, 1, "Indefinida"};         // Nop == 1 para iniciar a primeira instrução;

    etapaWB WB = {0, 0, 0, 0, 0, 1, "Indefinida"};                    // Nop == 1 para iniciar a primeira instrução;

    //? Inicializando estrutura de auxílio para as etapas do pipeline;
    ciclo clockAtual = {IF, ID, EX, MEM, WB};
    ciclo novoClock = clockAtual;
    
    //? Declarando variáveis auxiliares para etapas do pipeline;
	bitset<32> Instruction;        // Variável para armazenar a instrução;
	bitset<32> Ins;                // Variável auxiliar para instrução;
	string opcode;                 // Opcode da instrução;
	string func;                   // Campo Func;
	bitset<5> rs;                  // Registrador rs;
	bitset<5> rt;                  // Registrador rt;
	bitset<5> rd;                  // Registrador rd;
    bitset<16> immediate;          // Immediate das instruções do tipo I;

    cycle = 0;

    //? Começando execução do pipeline;
    while(1){

        //? Etapa IF ------------------------------------------------------------------------------------------------------------------
        if (clockAtual.IF.nop == 0){
            // Lendo instrução da memória;
            Instruction = memoria[clockAtual.IF.PC.to_ulong()/4];         

            // Se a instrução for diferente de zero, atualiza PC;
            if (Instruction != 0){

                novoClock.IF.PC = clockAtual.IF.PC.to_ulong() + 4;
                novoClock.IF.nop = 0;                               
            }
            // Caso contrário, no = 1;
            else{

                clockAtual.IF.nop = 1;
                
                novoClock.IF.PC = clockAtual.IF.PC.to_ulong();                
                novoClock.IF.nop = 1;                
            }
            // Passando a instrução para a próxima etapa;
            novoClock.ID.instrucao = Instruction;            
        }
        // Atualizando ciclo;
        novoClock.ID.nop = clockAtual.IF.nop;
        clockAtual = novoClock;

        // Se todos os nop forem iguais a 1, encerra o pipeline;
        if (clockAtual.IF.nop && clockAtual.ID.nop && clockAtual.EX.nop && clockAtual.MEM.nop && clockAtual.WB.nop)
            break;

        //? Etapa ID ------------------------------------------------------------------------------------------------------------------
        if (clockAtual.ID.nop == 0){
            
            // Atualizando variáveis para decodificação;
            Ins = clockAtual.ID.instrucao;
            opcode = Ins.to_string().substr(26,6);	
            func = Ins.to_string().substr(0,6);

            // Atualizando registradores rs, rt, rd e immediate;
            rs = bitset<5>(Ins.to_string().substr(21,5));
            novoClock.EX.rs = rs;
            novoClock.EX.regA = registradores[rs.to_ulong()];   
            
            rt = bitset<5>(Ins.to_string().substr(16,5));
            novoClock.EX.rt = rt;
            novoClock.EX.regB = registradores[rt.to_ulong()];
            
            immediate = bitset<16>(Ins.to_string().substr(0,16)); 
            novoClock.EX.Immediate = immediate;
            
            rd = bitset<5>(Ins.to_string().substr(11,5));  
            
            // Verificando opcodes;
            // Caso opcode == 000000, a instrução é do tipo R, será necessário verificar o func;
            if (opcode == "000000"){                 
                novoClock.EX.endereco = rd; 
                
                novoClock.EX.tipo_I = 0;

                if(func == "100000"){
                    cout << "add" << endl;
                    novoClock.EX.nome = "add";
                    novoClock.EX.aluOp = bitset<2>("10");
                }
                else{
                    if(func == "100010"){
                        cout << "sub" << endl;
                        novoClock.EX.nome = "subtract";
                        novoClock.EX.aluOp = bitset<2>("10");
                    }
                    else{
                        if(func == "100100"){
                            cout << "and" << endl;
                            novoClock.EX.nome = "AND";
                            novoClock.EX.aluOp = bitset<2>("10");
                        }
                        else{
                            if(func == "100101"){
                                cout << "or" << endl;
                                novoClock.EX.nome = "OR";
                                novoClock.EX.aluOp = bitset<2>("10");
                            }
                            else{
                                if(func == "101010"){
                                    cout << "slt" << endl;
                                    novoClock.EX.nome = "slt";
                                    novoClock.EX.aluOp = bitset<2>("10");
                                }
                                else{
                                    cout << "sll" << endl;
                                    novoClock.EX.nome = "sll";
                                    novoClock.EX.aluOp = bitset<2>("10");
                                }
                            }
                        }
                    }
                }              
            }            

            else if (opcode == "100011"){
                novoClock.EX.nome = "lw";                
                
                novoClock.EX.endereco = rt;
                
                novoClock.EX.tipo_I = 1;               
                novoClock.EX.aluOp = bitset<2>("01");
                novoClock.EX.regWrite = 1;                
                novoClock.EX.memRead = 1;
                novoClock.EX.memWrite = 0;                      
            }

            else if (opcode == "101011"){
                novoClock.EX.nome = "sw";       
                
                novoClock.EX.endereco = rt;

                novoClock.EX.tipo_I = 1;                
                novoClock.EX.aluOp = bitset<2>("01");;
                novoClock.EX.regWrite = 0;                
                novoClock.EX.memRead = 0;
                novoClock.EX.memWrite = 1;                 
            }
            
            else if (opcode == "000100"){
                novoClock.EX.nome = "beq";
                
                novoClock.EX.endereco = 0;
                
                novoClock.EX.tipo_I = 1;
                novoClock.EX.aluOp = bitset<2>("01");;
                novoClock.EX.regWrite = 0;
                novoClock.EX.memRead = 0;
                novoClock.EX.memWrite = 0; 

                bitset<32> aux1 = registradores[rs.to_ulong()];
                bitset<32> aux2 = registradores[rt.to_ulong()];
                
                if (aux1 == aux2){

                    novoClock.EX.nop = 0;
                    novoClock.ID.nop = 1;
                    
                    novoClock.IF.PC = clockAtual.IF.PC.to_ulong() + bitset<30>(extensorSinal(immediate).to_string().substr(2,30)).to_ulong()*4;
                    novoClock.IF.nop = 0;
                         
                    clockAtual = novoClock;
                    cycle ++;
                    
                    continue;                                       
                }
            }
            else if (opcode == "001000"){
                novoClock.EX.nome = "addi";

                novoClock.EX.tipo_I = 1;
                novoClock.EX.aluOp = bitset<2>("00");
                novoClock.EX.regWrite = 1;
                novoClock.EX.memRead = 0;
                novoClock.EX.memWrite = 0; 
            }
            else if (opcode == "000101"){
                novoClock.EX.nome = "bne";
                
                novoClock.EX.endereco = 0;
                
                novoClock.EX.tipo_I = 1;
                novoClock.EX.aluOp = bitset<2>("01");;
                novoClock.EX.regWrite = 0;
                novoClock.EX.memRead = 0;
                novoClock.EX.memWrite = 0; 

                bitset<32> aux1 = registradores[rs.to_ulong()];
                bitset<32> aux2 = registradores[rt.to_ulong()];
                
                if (aux1 != aux2){

                    novoClock.EX.nop = 0;
                    novoClock.ID.nop = 1;
                    
                    novoClock.IF.PC = clockAtual.IF.PC.to_ulong() + bitset<30>(extensorSinal(immediate).to_string().substr(2,30)).to_ulong()*4;
                    novoClock.IF.nop = 0;
                        
                    clockAtual = novoClock;
                    cycle ++;
                    
                    continue;                                       
                }
            }
            else if (opcode == "000010"){
                novoClock.EX.nome = "jump";

                novoClock.EX.tipo_I = 0;
                novoClock.EX.regWrite = 0;
                novoClock.EX.memRead = 0;
                novoClock.EX.memWrite = 0;
            }
            else if (opcode == "000011"){
                novoClock.EX.nome = "jump and link";

                novoClock.EX.tipo_I = 0;
                novoClock.EX.regWrite = 0;
                novoClock.EX.memRead = 0;
                novoClock.EX.memWrite = 0;
            }
            else {
                novoClock.EX.nome = "jump register";

                novoClock.EX.tipo_I = 0;
                novoClock.EX.regWrite = 0;
                novoClock.EX.memRead = 0;
                novoClock.EX.memWrite = 0;
            } 
            
            if ((clockAtual.EX.nop == 0) && (clockAtual.EX.memRead == 1)){

                if ((clockAtual.EX.endereco == rs) || ((clockAtual.EX.endereco == rt) && (novoClock.EX.tipo_I == 0))){

                    novoClock.EX.nop = 1;

                    novoClock.ID = clockAtual.ID;
                    novoClock.IF = clockAtual.IF;

                    clockAtual = novoClock;
                    cycle ++;
                    continue;
                }  
            }
        }
        novoClock.EX.nop = clockAtual.ID.nop;

        //? Etapa EX ------------------------------------------------------------------------------------------------------------------
        if (clockAtual.EX.nop == 0)
        {    
            // Verifica se existe hazzard;               
            if ((clockAtual.WB.nop == 0) && (clockAtual.WB.regWrite == 1) && (clockAtual.WB.endereco == clockAtual.EX.rs)){

                clockAtual.EX.regA = clockAtual.WB.info;
            }
            
            // Verifica se existe hazzard;
            if ((clockAtual.WB.nop == 0) && (clockAtual.WB.regWrite == 1) && (clockAtual.WB.endereco == clockAtual.EX.rt)){

                if (((clockAtual.EX.tipo_I == 0) && (clockAtual.EX.regWrite == 1)) || (clockAtual.EX.memWrite == 1)){

                    clockAtual.EX.regB = clockAtual.WB.info;               
                }
            }
            
            // Verifica se existe hazzard;
            if ((clockAtual.MEM.nop == 0) && (clockAtual.MEM.memRead == 0) && (clockAtual.MEM.memWrite == 0) && (clockAtual.MEM.regWrite == 1) && (clockAtual.MEM.endereco == clockAtual.EX.rs)){ 

                clockAtual.EX.regA = clockAtual.MEM.resultadoALU;
            }
            
            // Verifica se existe hazzard;
            if ((clockAtual.MEM.nop == 0) && (clockAtual.MEM.memRead == 0) && (clockAtual.MEM.memWrite == 0) && (clockAtual.MEM.regWrite == 0) && (clockAtual.MEM.endereco == clockAtual.EX.rt)){

                if ((clockAtual.EX.tipo_I == 0) && (clockAtual.EX.regWrite == 1)){

                    clockAtual.EX.regB = clockAtual.MEM.resultadoALU;
                }
            }            
            
            if (clockAtual.EX.tipo_I == 0){

                if (clockAtual.EX.regWrite == 1){

                    if (clockAtual.EX.aluOp == 1){

                        novoClock.MEM.resultadoALU = clockAtual.EX.regA.to_ulong() + clockAtual.EX.regB.to_ulong();               
                    }
                    else if (clockAtual.EX.aluOp == 0){

                        novoClock.MEM.resultadoALU = clockAtual.EX.regA.to_ulong() - clockAtual.EX.regB.to_ulong();              
                    }
                }
                else{
                    novoClock.MEM.resultadoALU = 0;

                    if(opcode == "000010"){
                        cout << "Jump" << endl;
                        novoClock.IF.PC = bitset<32>(rd.to_string());
                    }
                    else{
                        if(opcode == "000011"){
                            cout << "Jump and link" << endl;
                            registradores[31] = bitset<32>(clockAtual.IF.PC.to_ulong() + 8);
                            novoClock.IF.PC = bitset<32>(rd.to_string());
                        }
                        else{
                            if(opcode == "100000"){
                                cout << "Jump register" << endl;
                                novoClock.IF.PC = registradores[rs.to_ulong()];
                            }
                        }
                    }
                }
            }
            else if (clockAtual.EX.tipo_I == 1){
                novoClock.MEM.resultadoALU = clockAtual.EX.regA.to_ulong() + extensorSinal(clockAtual.EX.Immediate).to_ulong();
            }
            
            novoClock.MEM.data_forwarding = clockAtual.EX.regB;
            novoClock.MEM.rs = clockAtual.EX.rs;
            novoClock.MEM.rt = clockAtual.EX.rt;            
            novoClock.MEM.endereco = clockAtual.EX.endereco;              
            novoClock.MEM.regWrite = clockAtual.EX.regWrite;           
            novoClock.MEM.memRead = clockAtual.EX.memRead;
            novoClock.MEM.memWrite = clockAtual.EX.memWrite;           
        }
        novoClock.MEM.nop = clockAtual.EX.nop;        
        novoClock.MEM.nome = clockAtual.EX.nome;

        //? Etapa MEM ------------------------------------------------------------------------------------------------------------------
        if (clockAtual.MEM.nop == 0){
            if (clockAtual.MEM.memRead == 1){

                novoClock.WB.info = bitset<32>(memoria[clockAtual.MEM.resultadoALU.to_ulong()]);
            }
            else if (clockAtual.MEM.memWrite == 1){

                // Verifica se existe hazzard;
                if ((clockAtual.WB.nop == 0) && (clockAtual.WB.regWrite == 1) && (clockAtual.WB.endereco == clockAtual.MEM.rt)){

                    clockAtual.MEM.data_forwarding = clockAtual.WB.info;    
                }
                
                memoria[clockAtual.MEM.resultadoALU.to_ulong()] = bitset<32>(clockAtual.MEM.data_forwarding.to_string());

                novoClock.WB.info = clockAtual.MEM.data_forwarding;
            }
            else if (clockAtual.MEM.regWrite == 1){
                novoClock.WB.info = clockAtual.MEM.resultadoALU;
            }   

            novoClock.WB.rs = clockAtual.MEM.rs;
            novoClock.WB.rt = clockAtual.MEM.rt;             
            novoClock.WB.endereco = clockAtual.MEM.endereco;                      
            novoClock.WB.regWrite = clockAtual.MEM.regWrite;             
        }
        novoClock.WB.nop = clockAtual.MEM.nop;  
        novoClock.WB.nome = clockAtual.MEM.nome;

        //? Etapa WB ------------------------------------------------------------------------------------------------------------------
        if (clockAtual.WB.nop == 0){
            if (1 == clockAtual.WB.regWrite){
                registradores[clockAtual.WB.endereco.to_ulong()] = clockAtual.WB.info; 
            }         
        }
        imprimePipeLine(outfile, novoClock);      
        cycle ++;
    }
    outfile.close();
}