#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>


using namespace std;

//Variaveis Globais

bitset<32> memoria[512];      //Array que contem em cada posicao contem uma instrucao
bitset<8> dataMem[512];       //Array para a memoria de dados.

bitset<32> registradores[32]; //Array que contem em cada posicao um registrador, o array todo corresponde ao banco de registradores

//Sinais de Controle do Pipeline
int regDst, aluOp1, aluOp0, aluSrc, branch, memRead, memWrite, regWrite, memToReg; 

bitset<32> sign_extend(bitset<16> Imm)
{
    string se0 = "0000000000000000";
    string se1 = "1111111111111111";
    string immediate = Imm.to_string();
    bitset<32> se_imm;
    
    if (immediate.substr(0,1) == "0")							//sign extention
    {
        se0 += immediate;
        se_imm = bitset<32>(se0);
    }					
    else
    {
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
    string      INS;           // Nome da intrução;
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
    string      INS;              // Nome da intrução;
};

struct etapaWB {
    bitset<32>  info;          // Dados a serem salvos no banco de registradores;
    bitset<5>   rs;            // Registrador rs;
    bitset<5>   rt;            // Registrador rt;
    bitset<5>   endereco;      // Endereço de memória para auxiliar leitura e escrita;
    bool        regWrite;      // Sinal de controle RegWrite;
    bool        nop;           // Variável para tratamento dos Hazzards;
    string      INS;           // Nome da intrução;
};

struct stateStruct {
    etapaIF    IF;   // Etapa 1;
    etapaID    ID;   // Etapa 2;
    etapaEX    EX;   // Etapa 3;
    etapaMEM   MEM;  // Etapa 4;
    etapaWB    WB;   // Etapa 5;
};

//?------------------------------------------------------------------------------------------------------------------------------------

void pipeline(){

    //? Inicializando etapas do pipeline com seus respectivos sinais de controle;
    etapaIF IF = {0, 0};                                     // Nop == 0 para iniciar a primeira instrução;
    
    etapaID ID = {0, 1};                                     // Nop == 1 para iniciar a primeira instrução;
    
    etapaEX EX = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, "X"};  // Nop == 1 para iniciar a primeira instrução;
    
    etapaMEM MEM = {0, 0, 0, 0, 0, 0, 0, 0, 1, "X"};         // Nop == 1 para iniciar a primeira instrução;

    etapaWB WB = {0, 0, 0, 0, 0, 1, "X"};                    // Nop == 1 para iniciar a primeira instrução;

    //? Inicializando estrutura de auxílio para as etapas do pipeline;
    stateStruct state = {IF, ID, EX, MEM, WB};  
    stateStruct newState = state;
    
    //? Declarando variáveis auxiliares para etapas do pipeline;
	bitset<32> Instruction;        // Variável para armazenar a instrução;
	bitset<32> Instr;              // 
	string opcode;                 // Opcode da instrução;
	string func;                   // Campo Func;
	bitset<5> rs;                  // Registrador rs;
	bitset<5> rt;                  // Registrador rt;
	bitset<5> rd;                  // Registrador rd;
    bitset<16> immediate;          // Immediate das instruções do tipo I;

    //? Inicia no ciclo de clock 0;
    int cycle = 0;

    //? Começando execução do pipeline;
    while(1){
        //? Etapa WB ------------------------------------------------------------------------------------------------------------------
        // Se WB está ociosa, 
        if (0 == state.WB.nop){
            if (1 == state.WB.regWrite){
                registradores[state.WB.endereco.to_ulong()] = state.WB.info; 
            }         
        }

        //? Etapa MEM ------------------------------------------------------------------------------------------------------------------
        if (state.MEM.nop == 0){
            if (state.MEM.memRead == 1){

                string datamem;
                datamem.append(dataMem[state.MEM.resultadoALU.to_ulong()].to_string());
                datamem.append(dataMem[state.MEM.resultadoALU.to_ulong()+1].to_string());
                datamem.append(dataMem[state.MEM.resultadoALU.to_ulong()+2].to_string());
                datamem.append(dataMem[state.MEM.resultadoALU.to_ulong()+3].to_string());

                newState.WB.info = bitset<32>(datamem);
            }
            else if (state.MEM.memWrite == 1){

                if ((state.WB.nop == 0) && (state.WB.regWrite == 1) && (state.WB.endereco == state.MEM.rt)){

                    state.MEM.data_forwarding = state.WB.info;    
                    cout<<"MEM-MEM sw Forwarding"<<endl;
                }
                
                dataMem[state.MEM.resultadoALU.to_ulong()] = bitset<8>(state.MEM.data_forwarding.to_string().substr(0,8));
                dataMem[state.MEM.resultadoALU.to_ulong()+1] = bitset<8>(state.MEM.data_forwarding.to_string().substr(8,8));
                dataMem[state.MEM.resultadoALU.to_ulong()+2] = bitset<8>(state.MEM.data_forwarding.to_string().substr(16,8));
                dataMem[state.MEM.resultadoALU.to_ulong()+3] = bitset<8>(state.MEM.data_forwarding.to_string().substr(24,8));

                newState.WB.info = state.MEM.data_forwarding;
            }
            else if (state.MEM.regWrite == 1){

                newState.WB.info = state.MEM.resultadoALU;
            }   

            newState.WB.rs = state.MEM.rs;
            newState.WB.rt = state.MEM.rt;             
            newState.WB.endereco = state.MEM.endereco;                      
            newState.WB.regWrite = state.MEM.regWrite;             
        }
        newState.WB.nop = state.MEM.nop;  
        newState.WB.INS = state.MEM.INS;

        //? Etapa EX ------------------------------------------------------------------------------------------------------------------
        if (state.EX.nop == 0)
        {    
            // Verifica se vai escrever em Rs               
            if ((state.WB.nop == 0) && (state.WB.regWrite == 1) && (state.WB.endereco == state.EX.rs)){

                state.EX.regA = state.WB.info;
                cout<<"MEM-EX Rs Forwarding"<<endl;
            }
            
            // Verifica se vai escrever em Rt
            if ((state.WB.nop == 0) && (state.WB.regWrite == 1) && (state.WB.endereco == state.EX.rt)){

                if (((state.EX.tipo_I == 0) && (state.EX.regWrite == 1)) || (state.EX.memWrite == 1)){

                    state.EX.regB = state.WB.info;
                    cout<<"MEM-EX Rt Forwarding"<<endl;                
                }
            }
            
            
            if ((state.MEM.nop == 0) && (state.MEM.memRead == 0) && (state.MEM.memWrite == 0) && (state.MEM.regWrite == 1) && (state.MEM.endereco == state.EX.rs)){ 

                state.EX.regA = state.MEM.resultadoALU;
                cout<<"EX-EX Rs Forwarding"<<endl;
            }
            
            
            if ((state.MEM.nop == 0) && (state.MEM.memRead == 0) && (state.MEM.memWrite == 0) && (state.MEM.regWrite == 0) && (state.MEM.endereco == state.EX.rt)){

                if ((state.EX.tipo_I == 0) && (state.EX.regWrite == 1)){

                    state.EX.regB = state.MEM.resultadoALU;
                    cout<<"EX-EX Rt Forwarding"<<endl; 
                }
            }            
            
            if (state.EX.tipo_I == 0){

                if (state.EX.regWrite == 1){

                    if (state.EX.aluOp == 1){

                        newState.MEM.resultadoALU = state.EX.regA.to_ulong() + state.EX.regB.to_ulong();               
                    }
                    else if (state.EX.aluOp == 0){

                        newState.MEM.resultadoALU = state.EX.regA.to_ulong() - state.EX.regB.to_ulong();              
                    }
                }
                else{
                    newState.MEM.resultadoALU = 0; //case of branch
                }
            }
            else if (state.EX.tipo_I == 1){

                newState.MEM.resultadoALU = state.EX.regA.to_ulong() + sign_extend(state.EX.Immediate).to_ulong();
            }
            
            newState.MEM.data_forwarding = state.EX.regB;
            newState.MEM.rs = state.EX.rs;
            newState.MEM.rt = state.EX.rt;            
            newState.MEM.endereco = state.EX.endereco;              
            newState.MEM.regWrite = state.EX.regWrite;           
            newState.MEM.memRead = state.EX.memRead;
            newState.MEM.memWrite = state.EX.memWrite;           
        }
        newState.MEM.nop = state.EX.nop;        
        newState.MEM.INS = state.EX.INS;

        //? Etapa ID ------------------------------------------------------------------------------------------------------------------
        if (state.ID.nop == 0){

            Instr = state.ID.instrucao;
            opcode = Instr.to_string().substr(0,6);		//decode instruction
            func = Instr.to_string().substr(26,6);

            rs = bitset<5>(Instr.to_string().substr(6,5));
            newState.EX.rs = rs;
            newState.EX.regA = registradores[rs.to_ulong()];   
            
            rt = bitset<5>(Instr.to_string().substr(11,5));
            newState.EX.rt = rt;
            newState.EX.regB = registradores[rt.to_ulong()];
            
            immediate = bitset<16>(Instr.to_string().substr(16,16)); 
            newState.EX.Immediate = immediate;
            
            rd = bitset<5>(Instr.to_string().substr(16,5));  
            
            if (opcode == "000000"){                 
                newState.EX.endereco = rd; 
                
                newState.EX.tipo_I = 0;

                if(func == "100000"){
                    newState.EX.INS = "add";
                    newState.EX.aluOp = bitset<2>("10");
                }
                else{
                    if(func == "100010"){
                        newState.EX.INS = "subtract";
                        newState.EX.aluOp = bitset<2>("10");
                    }
                    else{
                        if(func == "100100"){
                            newState.EX.INS = "AND";
                            newState.EX.aluOp = bitset<2>("10");
                        }
                        else{
                            if(func == "100101"){
                                newState.EX.INS = "OR";
                                newState.EX.aluOp = bitset<2>("10");
                            }
                            else{
                                if(func == "101010"){
                                    newState.EX.INS = "slt";
                                    newState.EX.aluOp = bitset<2>("10");
                                }
                                else{
                                    newState.EX.INS = "sll";
                                    newState.EX.aluOp = bitset<2>("10");
                                }
                            }
                        }
                    }
                }              
            }            

            else if (opcode == "100011"){
                newState.EX.INS = "lw";                
                
                newState.EX.endereco = rt;
                
                newState.EX.tipo_I = 1;               
                newState.EX.aluOp = bitset<2>("01");
                newState.EX.regWrite = 1;                
                newState.EX.memRead = 1;
                newState.EX.memWrite = 0;                      
            }

            else if (opcode == "101011"){
                newState.EX.INS = "sw";       
                
                newState.EX.endereco = rt;

                newState.EX.tipo_I = 1;                
                newState.EX.aluOp = bitset<2>("01");;
                newState.EX.regWrite = 0;                
                newState.EX.memRead = 0;
                newState.EX.memWrite = 1;                 
            }
            
            else if (opcode == "000100"){
                newState.EX.INS = "beq";
                
                newState.EX.endereco = 0;
                
                newState.EX.tipo_I = 1;
                newState.EX.aluOp = bitset<2>("01");;
                newState.EX.regWrite = 0;
                newState.EX.memRead = 0;
                newState.EX.memWrite = 0; 

                bitset<32> aux1 = registradores[rs.to_ulong()];
                bitset<32> aux2 = registradores[rt.to_ulong()];
                
                if (aux1 == aux2){

                    cout<<"Branch not taken"<<endl;
                    newState.EX.nop = 0;
                    newState.ID.nop = 1;
                    
                    newState.IF.PC = state.IF.PC.to_ulong() + bitset<30>(sign_extend(immediate).to_string().substr(2,30)).to_ulong()*4;
                    newState.IF.nop = 0;
                    
                    //printState(newState, cycle);     
                    state = newState;
                    cycle ++;
                    
                    continue;                                       
                }
                cout<<"Branch taken"<<endl;
            }
            else if (opcode == "001000"){
                newState.EX.INS = "addi";

                newState.EX.tipo_I = 1;
                newState.EX.aluOp = bitset<2>("00");
                newState.EX.regWrite = 1;
                newState.EX.memRead = 0;
                newState.EX.memWrite = 0; 
            }
            else if (opcode == "000101"){
                newState.EX.INS = "bne";
                
                newState.EX.endereco = 0;
                
                newState.EX.tipo_I = 1;
                newState.EX.aluOp = bitset<2>("01");;
                newState.EX.regWrite = 0;
                newState.EX.memRead = 0;
                newState.EX.memWrite = 0; 

                bitset<32> aux1 = registradores[rs.to_ulong()];
                bitset<32> aux2 = registradores[rt.to_ulong()];
                
                if (aux1 != aux2){

                    cout<<"Branch not taken"<<endl;
                    newState.EX.nop = 0;
                    newState.ID.nop = 1;
                    
                    newState.IF.PC = state.IF.PC.to_ulong() + bitset<30>(sign_extend(immediate).to_string().substr(2,30)).to_ulong()*4;
                    newState.IF.nop = 0;
                    
                    //printState(newState, cycle);     
                    state = newState;
                    cycle ++;
                    
                    continue;                                       
                }
                cout<<"Branch taken"<<endl;
            }
            else if (opcode == "000010"){
                newState.EX.INS = "jump";

                newState.EX.tipo_I = 0;
                newState.EX.regWrite = 0;
                newState.EX.memRead = 0;
                newState.EX.memWrite = 0;

                //newState.IF.PC = 
            }
            else if (opcode == "000011"){
                newState.EX.INS = "jump and link";

                newState.EX.tipo_I = 0;
                newState.EX.regWrite = 0;
                newState.EX.memRead = 0;
                newState.EX.memWrite = 0;

                //newState.IF.PC =
            }
            else {
                newState.EX.INS = "jump register";

                newState.EX.tipo_I = 0;
                newState.EX.regWrite = 0;
                newState.EX.memRead = 0;
                newState.EX.memWrite = 0;

                //newState.IF.PC =
            } 
            
            if ((state.EX.nop == 0) && (state.EX.memRead == 1)){

                if ((state.EX.endereco == rs) || ((state.EX.endereco == rt) && (newState.EX.tipo_I == 0))){

                    newState.EX.nop = 1;

                    newState.ID = state.ID;
                    newState.IF = state.IF;

                    //printState(newState, cycle);
                    state = newState;
                    cycle ++;
                    cout<<"Stall"<<endl;
                    continue;
                }  
            }
        }
        newState.EX.nop = state.ID.nop;

        //? Etapa IF ------------------------------------------------------------------------------------------------------------------
        if (state.IF.nop == 0){

            //TODO cout<<"PC:\t"<<state.IF.PC<<endl;
            Instruction = memoria[state.IF.PC.to_ulong()/4];          
            //cout<<"Instruction:\t"<<Instruction<<endl;

            if (Instruction != 0xffffffff){

                newState.IF.PC = state.IF.PC.to_ulong() + 4;
                newState.IF.nop = 0;                               
            }
            else{

                state.IF.nop = 1;
                
                newState.IF.PC = state.IF.PC.to_ulong();                
                newState.IF.nop = 1;
                //cout<<"PC:\t"<<state.IF.PC<<endl;                
            }
            
            newState.ID.instrucao = Instruction;            
        }
        newState.ID.nop = state.IF.nop;

        
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        //printState(newState, cycle);
       
        state = newState;
                
        cycle ++;
    }
}