#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>

using namespace std;
long long int memoria[256], instruct;
int registradores[32];
int reg1, reg2, creg1, creg2, Aluout, regW, pc, funct, reg3m0, reg3m1, opcode, shift, auxI, offseti;
string aux, aux2;
int cclock = 1;
struct controle
{
    int PcSrc;
    bool PcWrite;
    int PcWriteCond;
    int RegDst;
    bool Irwrite;
    bool MemToReg;
    bool MemWrite;
    bool MemRead;
    bool IoD;
    bool RegWrite;
    bool Alusrca;
    int Alusrcb;
    int Aluop;
};
struct controle controle;
long long int bytesToInteger(std::string bytes)
{
    long long int total = 0;
    long long int k = 0;
    for (int i = 0; bytes[i++] != '\0'; k++)
        ;
    for (int i = 0; bytes[i] != '\0'; i++)
    {
        total += pow(2, (k - i - 1)) * ((bytes[i] - '0'));
    };
    return total;
}
long long int bytesToIntegerNeg(std::string bytes)
{
    long long int total = 0;
    long long int k = 0;
    if (bytes[0] == '0')
        return bytesToInteger(bytes);
    for (int i = 0; bytes[i++] != '\0'; k++)
        ;
    for (int i = 0; bytes[i] != '\0'; i++)
    {
        total += pow(2, (k - i - 1)) * ((bytes[0] == 0) ? (bytes[i] - '0') : (!(bool)(bytes[i] - '0')));
    };
    if (bytes[0] == '1')
    {
        total = -(total + 1);
    }
    return total;
}
std::string integerToByte(long long int n, long long int numBytes)
{
    long long int num = n;
    std::string k = std::string(" ", numBytes);
    int i = 0;
    for (long long int aux = num; aux > 0; i++)
    {
        k = ((aux % 2) == 0 ? '0' : '1') + k;
        aux = aux / 2;
    }
    for (long long int aux = numBytes; aux > i; i++)
    {
        k = '0' + k;
    }
    k[i] = '\0';
    return k;
}
void leArquivoTexto(long long int memoria[], string arquivo)
{
    ifstream arq(arquivo);
    if (arq.is_open())
    {
        string str;
        int count = 1;
        int j = 0;
        while (getline(arq, str))
        {
            memoria[j] = bytesToInteger(str);
            j++;
            count++;
        }
    }
    else
        cerr << "ERRO: O arquivo nao pode ser aberto!" << endl;
}
void setControl(int op, int etapa)
{
    if (etapa == 0)
    {
        controle.RegDst = 0;
        controle.RegWrite = 0;
        controle.MemWrite = 0;
        controle.MemRead = 0;
        controle.Irwrite = 1;
        controle.IoD = 0;
        controle.Alusrca = 0;
        controle.Alusrcb = bytesToInteger("01");
        controle.Aluop = bytesToInteger("00");
        controle.PcSrc = bytesToInteger("00");
        controle.PcWrite = 1;
        controle.PcWriteCond = 0;
    }
    else if (etapa == 1)
    {
        controle.MemRead = 0;
        controle.Irwrite = 1;
        controle.IoD = 0;
        controle.Alusrca = 0;
        controle.Alusrcb = bytesToInteger("11");
        controle.Aluop = bytesToInteger("00");
        controle.PcSrc = bytesToInteger("00");
        controle.PcWrite = 1;
    }
    else if (etapa == 2)
    {
        switch (opcode)
        {
        case 0: // Tipo R
            controle.RegWrite = 1;
            controle.Irwrite = 1;
            controle.IoD = 0;
            controle.Alusrca = 1;
            controle.Alusrcb = bytesToInteger("00");
            controle.Aluop = bytesToInteger("10");
            controle.PcSrc = bytesToInteger("00");
            controle.PcWrite = 1;
            break;
        case 35: // LW
            controle.Alusrca = 1;
            controle.Alusrcb = bytesToInteger("10");
            controle.Aluop = bytesToInteger("00");
            break;
        case 43: // SW
            controle.Alusrca = 1;
            controle.Alusrcb = bytesToInteger("10");
            controle.Aluop = bytesToInteger("00");
            break;
        case 4: // BEQ
            controle.Alusrca = 1;
            controle.Alusrcb = bytesToInteger("00");
            controle.Aluop = bytesToInteger("01");
            controle.PcWriteCond = 1; // BOOL?
            controle.PcSrc = bytesToInteger("01");
            break;
        case 5: // BNE
            controle.Alusrca = 1;
            controle.Alusrcb = bytesToInteger("00");
            controle.Aluop = bytesToInteger("01");
            controle.PcWriteCond = 1; // BOOL?
            controle.PcSrc = bytesToInteger("01");
            break;
        case 2:
            controle.PcWrite = 1;
            controle.PcSrc = bytesToInteger("10");
            break;
        case 3:
            controle.PcWrite = 1;
            controle.MemToReg = 1;
            controle.RegWrite = 1;
            controle.PcSrc = bytesToInteger("10");
            controle.RegDst = bytesToInteger("10");
            break;
        case 8:
            controle.RegDst = 0;
            controle.Alusrca = 1;
            controle.Alusrcb = bytesToInteger("10");
            break;
        }
    }
    else if (etapa == 3)
    {
        switch (opcode)
        {
        case 0:
            controle.RegDst = 1;
            controle.RegWrite = 1;
            controle.MemToReg = 0;
            controle.Irwrite = 1;
            controle.IoD = 0;
            controle.Alusrca = 1;
            controle.Alusrcb = bytesToInteger("00");
            controle.Aluop = bytesToInteger("10");
            controle.PcSrc = bytesToInteger("00");
            controle.PcWrite = 1;
            break;
        case 35:
            controle.MemWrite = 0;
            controle.MemRead = 1;
            controle.IoD = 1;
            break;
        case 43:
            controle.MemWrite = 1;
            controle.IoD = 1;
            controle.Alusrcb = bytesToInteger("10");
            break;
        case 8:
            controle.RegWrite = 1;
            controle.MemToReg = 0;
            break;
        }
    }
    else if (etapa == 4)
    {
        controle.RegDst = 0;
        controle.RegWrite = 1;
        controle.MemToReg = 1;
    }
}
int Alu()
{
    int a, b;
    if (controle.Alusrca == 0)
        a = pc;
    else
        a = creg1;
    switch (controle.Alusrcb)
    {
    case 0:
        b = creg2;
        break;
    case 1:
        b = 4;
        break;
    case 3:
        b = offseti * 4;
        break;
    case 2:
        b = offseti;
        break;
    }
    switch (controle.Aluop)
    {
    case 0:
        Aluout = a + b;
        break;
    case 1:
        Aluout = a - b;
    case 2:
        switch (funct)
        {
        case 0:
            Aluout = a + b;
            break;
        case 2:
            Aluout = a - b;
            break;
        case 4:
            Aluout = a & b; // and
            break;
        case 5:
            Aluout = a | b; // or
            break;
        case 10:
            Aluout = ((a < b) ? 1 : 0); //slt
            break;
        }
        break;
    }
    return Aluout;
}
void printReg(ostream &file)
{
    for (int i = 0; i < 32; i++)
    {
        file << std::setfill('0') << std::setw(4) << " |R[" << i << "]=" << registradores[i] << "| ";
        if (i % 16 == 15)
            file << endl;
    }
    file << endl;
}
void printMem(ostream &file)
{
    for (int i = 0; i < 256; i++)
    {
        if (memoria[i] != -1)
        {
            file << std::setfill('0') << std::setw(4) << " |M[" << i << "]=" << memoria[i] << "| ";
            if (i % 16 == 15)
                file << endl;
        }
    }
    file << endl;
}
void printEtapaAux(ostream &o, int etapa)
{
    o << endl;
    o << "EXECUTANDO OPERACAO : " + integerToByte(instruct, 32).substr(0, 32) << " PC =
                                                                                 "<<pc<<endl;
        o
      << " ETAPA " << etapa << endl;
    o << " Clock " << cclock << endl;
    printReg(o);
    o << endl;
}
ofstream file;
void printControle(ostream &o)
{
    o << "PCSRC =" << controle.PcSrc;
    o << " PCWRITE =" << controle.PcWrite;
    o << " PCWRITECond =" << controle.PcWriteCond;
    o << " RegDst =" << controle.RegDst << endl;
    o << " Irwrite =" << controle.Irwrite;
    o << " MemToReg =" << controle.MemToReg;
    o << " IoD =" << controle.IoD;
    o << " RegWrite =" << controle.RegWrite << endl;
    o << " MemRead =" << controle.MemRead;
    o << " MemWrite =" << controle.MemWrite;
    o << " Alusrca =" << controle.Alusrca;
    o << " Alusrcb =" << controle.Alusrcb;
    o << " Aluop =" << controle.Aluop << endl;
}
bool parada = true;
void flushReg()
{
    for (int i = 0; i < 32; i++)
        registradores[i] = -100;
    cout << endl
         << "Registro Limpo" << endl;
}
void flushMem()
{
    for (int i = 0; i < 256; i++)
        memoria[i] = -1;
    cout << endl
         << "Registro Limpo" << endl;
}
void menuParada()
{
    int i;
    do
    {
        cout << "1-PROXIMA ETAPA" << endl
             << "2-FLUSH REGISTRADORES" << endl
             << "3-FLUSH
                MEMORIA ";
                cin >>
            i;
        if (i == 2)
            flushReg();
        if (i == 3)
            flushMem();
    } while (i != 1);
}
void printEtapa(int etapa)
{
    printEtapaAux(cout, etapa);
    printEtapaAux(file, etapa);
    printMem(file);
    printControle(file);
    if (parada)
        menuParada();
    cclock++;
}
int main()
{
    //inicializaçăo variaveis
    for (int i = 0; i < 256; i++)
        memoria[i] = -1;
    for (int i = 0; i < 32; i++)
        registradores[i] = -100;
    memoria[100 - 60] = 1;
    registradores[30] = 100;
    registradores[5] = 120;
    int opcao;
    cout << "1- Carga do arquivo " << endl;
    cout << "2- Entrar com instrucoes via teclado " << endl;
    cin >> opcao;
    if (opcao == 1)
    {
        string arquivo;
        cout << "Digite o nome do arquivo de instrucoes " << endl;
        cin >> arquivo;
        leArquivoTexto(memoria, arquivo);
    }
    if (opcao == 2)
    {
        string str;
        int insere = 1;
        int i = 0;
        while (insere == 1)
        {
            cout << "Digite uma instrucao: ";
            cin >> str;
            memoria[i] = bytesToInteger(str);
            cout << "instrucao armazenada" << endl;
            i++;
            cout << "0 PARA PARAR OU 1 PARA CONTINUAR" << endl;
            cin >> insere;
        }
    }
    cout << "1- Execucao seriada" << endl;
    cout << "2- Execucao direta " << endl;
    cin >> opcao;
    parada = opcao == 1;
    file.open("saida.txt");
    int k;
    pc = 0;
    do
    {
        //etapa 1
        instruct = memoria[pc / 4];
        if (instruct == -1)
            break;
        setControl(-1, 0);
        pc = Alu();
        printEtapa(1);
        //etapa 2
        setControl(-1, 1);
        string offset26;
        //leitura registradores
        reg1 = bytesToInteger(integerToByte(instruct, 32).substr(6, 5));
        reg2 = bytesToInteger(integerToByte(instruct, 32).substr(11, 5));
        reg3m1 = bytesToInteger(integerToByte(instruct, 32).substr(16, 5));
        reg3m0 = bytesToInteger(integerToByte(instruct, 32).substr(11, 5));
        shift = bytesToInteger(integerToByte(instruct, 32).substr(20, 5));
        aux = integerToByte(instruct, 32).substr(16, 16);
        offset26 = (integerToByte(instruct, 32).substr(6, 26));
        creg1 = registradores[reg1];
        creg2 = registradores[reg2];
        opcode = bytesToInteger(integerToByte(instruct, 32).substr(0, 6));
        char bit = aux[0];
        aux.insert(aux.begin(), 16, bit); //shift 16
        offseti = bytesToIntegerNeg(aux);
        Aluout = Alu();
        int desvio = Aluout;
        printEtapa(2);
        //etapa 3
        string bytespc = integerToByte(pc, 32);
        string offset32 = bytespc.substr(0, 4) + offset26 + "00";
        setControl(opcode, 2);
        funct = bytesToInteger(integerToByte(instruct, 32).substr(28, 4));
        if (controle.Aluop != 1)
            Aluout = Alu();
        if (controle.PcSrc == 2)
        {
            int pulo = bytesToIntegerNeg(offset32.substr(4, 28));
            if (controle.RegDst == 2)
                registradores[31] = pc;
            pc += pulo - 4;
            printEtapa(3);
            continue;
        }
        if (funct == 8)
        {
            pc = registradores[31];
            printEtapa(3);
            continue;
        }
        if (controle.PcWriteCond)
        {
            int Zero = Alu();
            if (opcode == 4)
                Zero = !Zero;
            if (Zero)
            {
                pc = desvio - 4;
            }
            printEtapa(3);
            continue;
        }
        printEtapa(3);
        //etapa 4
        setControl(opcode, 3);
        int indexEscrita;
        switch (controle.RegDst)
        {
        case 0:
            indexEscrita = reg3m0; // reg3m0 == rd;
            break;
        case 1:
            indexEscrita = reg3m1; // reg3m1 == rt;
            break;
        case 2:
            indexEscrita = 31;
            break;
        }
        int escrita;
        if (controle.MemToReg == 0)
            escrita = Aluout;
        else if (controle.RegDst == 2)
            escrita = pc;
        if (controle.MemWrite == 1)
        {
            memoria[Aluout] = creg2;
        }
        // else se memtoreg=1 escrita=leitura da memoria
        if (controle.RegWrite)
            registradores[indexEscrita] = escrita;
        int aux;
        printEtapa(4);
        if (controle.MemRead)
            aux = memoria[Aluout];
        else
            continue;
        //etapa 5
        registradores[reg2] = aux;
        printEtapa(5);
    } while (instruct != -1 && pc >= 0 && pc < 400);
    return 0;
}