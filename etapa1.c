#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

void calcular_ula(int32_t, int32_t, int, int, int, int, int, int, int32_t*, int*, FILE*);
void print_binario(FILE*, int32_t);

int main() {
    FILE* arq = fopen("entradas.txt", "r");
    FILE* logs = fopen("logs.txt", "w");
    if (!arq) {
        puts("Não foi possível abrir o arquivo");
        return 0;
    } 
    if (!logs) {
        puts("Não foi possível abrir o arquivo");
        return 0;
    } 

    int32_t A = 0b11111111111111111111111111111111;
    int32_t B = 0b00000000000000000000000000000001;
    int32_t s = 0;
    int vai_um = 0;
    int PC = 1;

    fprintf(logs, "A: ", A);
    print_binario(logs, A);
    fprintf(logs, "B: ");
    print_binario(logs, B);

    fprintf(logs, "\nComeço do programa\n");

    char bits[7];
    while (fscanf(arq, "%s", bits) == 1) {
        
        int F0 = bits[0] - '0';
        int F1 = bits[1] - '0';
        int ENA = bits[2] - '0';
        int ENB = bits[3] - '0';
        int INVA = bits[4] - '0';
        int INC = bits[5] - '0';

        fprintf(logs, "===============================================================");
        fprintf(logs, "\nCiclo %d\n", PC);
        fprintf(logs, "\nPC = %d\n", PC);
        fprintf(logs, "IR = %s\n", bits);
        calcular_ula(A, B, F0, F1, ENA, ENB, INVA, INC, &s, &vai_um, logs);
        PC++;
    }
    fprintf(logs, "===============================================================");
    fprintf(logs, "\nCiclo %d\n", PC);
    fprintf(logs, "\nPC = %d\n", PC);
    fprintf(logs, "Fim do arquivo");
}

void calcular_ula(int32_t A, int32_t B, int F0, int F1, int ENA, int ENB, int INVA, int INC, int32_t* s, int* vai_um, FILE* logs) {
    int32_t a = ENA ? A : 0; // verifica se ENA é 1 para manter o valor de A
    int32_t b = ENB ? B : 0; // verifica se ENB é 1 para manter o valor de B
    
    a = INVA ? ~a : a; // verifica se INVA é 1 para iverter o valor de A

    *vai_um = 0; // inicializa vai_um com 0

    if (F0 == 0 && F1 == 0) { // se F0 == 0 e F1 == 0, habilita AB
        *s = a & b;
    } else if (F0 == 0 && F1 == 1) { // se F0 == 0 e F1 == 1, habilita A + B
        *s = a | b;
    } else if (F0 == 1 && F1 == 0) { // se F0 == 1 e F1 == 0. habilita NOT B
        *s = ~b;
    } else if (F0 == 1 && F1 == 1) { // se F1 == 1 e F1 == 1, habilita a soma completa, A + B + INC

        /*
        Calcula a soma completa.
        Faz o casting de a e b para inteiros sem sinal, para preservar os 32 bits, e depois outro casting para um inteiro de 64 bits,
        acrescentando 32 bits 0 a esquerda.
        Após isso soma e guarda o resultado em soma_completa
        */
        int64_t soma_completa = (uint64_t)(uint32_t)a + (uint64_t)(uint32_t)b + INC;

        /*
        O valor de s será os primeiros 32 bits a direita.
        Para isso, soma a soma_completa 0xFFFFFFFF (32 bits 0 seguidos de 32 bits 1) a fim de preservar somente esses 32 bits.
        Faz o casting para um inteiro de 32 bits.
        */
        *s = (int32_t)(soma_completa & 0xFFFFFFFF);

        /*
        O valor de vai_um é justamente o bit que sobrou da soma_completa, o 33º bit,
        então deslocamos 32 bits para a direita da soma_completa, e encontramos nosso vai_um,
        o & 1 é justamente por segurança, ele limpa qualquer coisa que estiver acima dele.
        Faz o casting para um inteiro.
        */
        *vai_um = (int)(soma_completa >> 32) & 1;
    }

    fprintf(logs, "B = ");
    print_binario(logs, b);
    fprintf(logs, "A = ");
    print_binario(logs, a);
    fprintf(logs, "S = ");
    print_binario(logs, *s);
    fprintf(logs, "Vai-Um = %d\n", *vai_um);
}

void print_binario(FILE* logs, int32_t num) {
    // Faz um loop do bit 31 (mais à esquerda) até o bit 0 (mais à direita)
    for (int i = 31; i >= 0; i--) {
        // Desloca o número para a direita e isola o bit atual
        int bit = (num >> i) & 1;
        fprintf(logs, "%d", bit);
    }
    fprintf(logs, "\n");
}
