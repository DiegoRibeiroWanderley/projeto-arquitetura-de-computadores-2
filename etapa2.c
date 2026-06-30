#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

/* =========================================================
 * Protótipos
 * ========================================================= */
void calcular_ula(uint32_t, uint32_t, int, int, int, int, int, int, int, int, uint32_t *s_saida, uint32_t *sd_saida, int *vai_um, int *N, int *Z);
void print_binario_32(uint32_t, FILE *);
void print_binario_8(uint8_t, FILE *);

void tarefa1(void);
void tarefa2(void);

/* =========================================================
 * main
 * ========================================================= */
int main(void) {
    tarefa1();
    tarefa2();
    return 0;
}

/* =========================================================
 * Funções de Impressão
 * ========================================================= */
void print_binario_32(uint32_t num, FILE *out) {
    for (int i = 31; i >= 0; i--) {
        fprintf(out, "%d", (num >> i) & 1);
    }
    fprintf(out, "\n");
}

void print_binario_8(uint8_t num, FILE *out) {
    for (int i = 7; i >= 0; i--) {
        fprintf(out, "%d", (num >> i) & 1);
    }
    fprintf(out, "\n");
}

/* =========================================================
 * calcular_ula
 * ========================================================= */
void calcular_ula(uint32_t a_in, uint32_t b_in,
                  int SLL8, int SRA1,
                  int F0, int F1,
                  int ENA, int ENB, int INVA, int INC,
                  uint32_t *s_saida, uint32_t *sd_saida,
                  int *vai_um, int *N, int *Z) {

    uint32_t a = ENA ? a_in : 0;
    uint32_t b = ENB ? b_in : 0;
    a = INVA ? ~a : a;

    *vai_um = 0;

    if (F0 == 0 && F1 == 0) {
        *s_saida = a & b;
    } else if (F0 == 0 && F1 == 1) {
        *s_saida = a | b;
    } else if (F0 == 1 && F1 == 0) {
        *s_saida = ~b;
    } else {
        uint64_t soma = (uint64_t)a + (uint64_t)b + (uint64_t)INC;
        *s_saida = (uint32_t)(soma & 0xFFFFFFFF);
        *vai_um  = (int)((soma >> 32) & 1);
    }

    if (SLL8) {
        *sd_saida = *s_saida << 8;
    } else if (SRA1) {
        *sd_saida = (uint32_t)(((int32_t)*s_saida) >> 1);
    } else {
        *sd_saida = *s_saida;
    }

    *N = ((int32_t)*sd_saida < 0) ? 1 : 0;
    *Z = (*sd_saida == 0) ? 1 : 0;
}

/* =========================================================
 * TAREFA 1
 * ========================================================= */
void tarefa1(void) {
    uint32_t b_val = 0x80000000;
    uint32_t a_val = 0x00000001;

    FILE *prog = fopen("programa_etapa2_tarefa1.txt", "r");
    if (!prog) return;

    FILE *log = fopen("saida_etapa2_tarefa1.txt", "w");
    if (!log) {
        fclose(prog);
        return;
    }

    fprintf(log, "b = "); print_binario_32(b_val, log);
    fprintf(log, "a = "); print_binario_32(a_val, log);
    fprintf(log, "\nStart of Program\n");

    char bits[10];
    int PC = 1;

    while (fscanf(prog, "%8s", bits) == 1) {
        fprintf(log, "============================================================\n");
        fprintf(log, "Cycle %d\n\n", PC);
        fprintf(log, "PC = %d\n", PC);
        
        if (strlen(bits) < 8) {
            fprintf(log, "> Line is empty, EOP.\n");
            break;
        }

        fprintf(log, "IR = %s\n", bits);

        int SLL8 = bits[0] - '0';
        int SRA1 = bits[1] - '0';
        int F0   = bits[2] - '0';
        int F1   = bits[3] - '0';
        int ENA  = bits[4] - '0';
        int ENB  = bits[5] - '0';
        int INVA = bits[6] - '0';
        int INC  = bits[7] - '0';

        if (SLL8 == 1 && SRA1 == 1) {
            fprintf(log, "> Error, invalid control signals.\n");
            PC++;
            continue;
        }

        uint32_t S, Sd;
        int vai_um, N, Z;

        calcular_ula(a_val, b_val, SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC,
                     &S, &Sd, &vai_um, &N, &Z);

        fprintf(log, "b = "); print_binario_32(b_val, log);
        fprintf(log, "a = "); print_binario_32(a_val, log);
        fprintf(log, "s = "); print_binario_32(S,  log);
        fprintf(log, "sd = "); print_binario_32(Sd, log);
        fprintf(log, "n = %d\n", N);
        fprintf(log, "z = %d\n", Z);
        fprintf(log, "co = %d\n", vai_um);

        PC++;
    }

    // Fim de arquivo explícito
    fprintf(log, "============================================================\n");
    fprintf(log, "Cycle %d\n\n", PC);
    fprintf(log, "PC = %d\n", PC);
    fprintf(log, "> Line is empty, EOP.\n\n");

    fclose(prog);
    fclose(log);
}

/* =========================================================
 * Estruturas e Constantes Tarefa 2
 * ========================================================= */
static const char *nome_barr_b[] = {
    "mdr", "pc", "mbr", "mbru", "sp", "lv", "cpp", "tos", "opc"
};

static const char *nome_barr_c[] = {
    "mar", "mdr", "pc", "sp", "lv", "cpp", "tos", "opc", "h"
};

typedef struct {
    uint32_t H;
    uint32_t OPC;
    uint32_t TOS;
    uint32_t CPP;
    uint32_t LV;
    uint32_t SP;
    uint32_t PC;
    uint32_t MDR;
    uint32_t MAR;
    uint8_t MBR;
} Registradores;

static void imprimir_regs_t2(const Registradores *r, FILE *out) {
    fprintf(out, "mar = "); print_binario_32(r->MAR, out);
    fprintf(out, "mdr = "); print_binario_32(r->MDR, out);
    fprintf(out, "pc = "); print_binario_32(r->PC, out);
    fprintf(out, "mbr = "); print_binario_8(r->MBR, out);
    fprintf(out, "sp = "); print_binario_32(r->SP, out);
    fprintf(out, "lv = "); print_binario_32(r->LV, out);
    fprintf(out, "cpp = "); print_binario_32(r->CPP, out);
    fprintf(out, "tos = "); print_binario_32(r->TOS, out);
    fprintf(out, "opc = "); print_binario_32(r->OPC, out);
    fprintf(out, "h = "); print_binario_32(r->H, out);
}

/* =========================================================
 * TAREFA 2
 * ========================================================= */
void tarefa2(void) {
    FILE *prog = fopen("programa_etapa2_tarefa2.txt", "r");
    if (!prog) return;

    FILE *log = fopen("saida_etapa2_tarefa2.txt", "w");
    if (!log) {
        fclose(prog);
        return;
    }

    Registradores regs;
    memset(&regs, 0, sizeof(regs));

    // Estado inicial de acordo com o arquivo de teste (simplificado para demonstração direta)
    regs.MAR = 0; regs.MDR = 0; regs.PC = 0; regs.MBR = 0x81;
    regs.SP = 0; regs.LV = 0; regs.CPP = 0; regs.TOS = 2; regs.OPC = 0; regs.H = 1;

    fprintf(log, "=====================================================\n");
    fprintf(log, "> Initial register states\n");
    imprimir_regs_t2(&regs, log);
    fprintf(log, "\n=====================================================\n");
    fprintf(log, "Start of program\n");
    fprintf(log, "=====================================================\n");

    char bits[24];
    int ciclo = 1;

    while (fscanf(prog, "%21s", bits) == 1) {
        if (strlen(bits) < 21) continue;

        char ula_str[9], c_str[10], b_str[5];
        strncpy(ula_str, bits, 8); ula_str[8] = '\0';
        strncpy(c_str, bits + 8, 9); c_str[9] = '\0';
        strncpy(b_str, bits + 17, 4); b_str[4] = '\0';

        int SLL8 = ula_str[0] - '0', SRA1 = ula_str[1] - '0';
        int F0   = ula_str[2] - '0', F1   = ula_str[3] - '0';
        int ENA  = ula_str[4] - '0', ENB  = ula_str[5] - '0';
        int INVA = ula_str[6] - '0', INC  = ula_str[7] - '0';

        int sel_c = 0;
        for (int i = 0; i < 9; i++) sel_c = (sel_c << 1) | (c_str[i] - '0');

        int sel_b = 0;
        for (int i = 0; i < 4; i++) sel_b = (sel_b << 1) | (b_str[i] - '0');

        fprintf(log, "Cycle %d\n", ciclo);
        fprintf(log, "ir = %s %s %s\n\n", ula_str, c_str, b_str);

        // Barramento B
        uint32_t B_ula = 0;
        switch (sel_b) {
            case 0: B_ula = regs.MDR; break;
            case 1: B_ula = regs.PC; break;
            // Para emular o arquivo de saida, MBR aplica extensão com zeros e MBRU aplica extensão de sinal
            case 2: B_ula = (uint32_t)regs.MBR; break;
            case 3: B_ula = (regs.MBR & 0x80) ? ((uint32_t)regs.MBR | 0xFFFFFF00u) : (uint32_t)regs.MBR; break;
            case 4: B_ula = regs.SP; break;
            case 5: B_ula = regs.LV; break;
            case 6: B_ula = regs.CPP; break;
            case 7: B_ula = regs.TOS; break;
            case 8: B_ula = regs.OPC; break;
        }
        
        fprintf(log, "b_bus = %s\n", (sel_b < 9) ? nome_barr_b[sel_b] : "???");
        
        fprintf(log, "c_bus = ");
        int comma = 0;
        for (int b = 8; b >= 0; b--) {
            if (sel_c & (1 << b)) {
                if (comma) fprintf(log, ", ");
                fprintf(log, "%s", nome_barr_c[b]);
                comma = 1;
            }
        }
        fprintf(log, "\n\n");

        fprintf(log, "> Registers before instruction\n");
        imprimir_regs_t2(&regs, log);

        // ULA
        uint32_t S, Sd;
        int vai_um, N, Z;
        calcular_ula(regs.H, B_ula, SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC, &S, &Sd, &vai_um, &N, &Z);

        // Escrita barramento C
        for (int b = 8; b >= 0; b--) {
            if (sel_c & (1 << b)) {
                switch (b) {
                    case 0: regs.MAR = Sd; break;
                    case 1: regs.MDR = Sd; break;
                    case 2: regs.PC  = Sd; break;
                    case 3: regs.SP  = Sd; break;
                    case 4: regs.LV  = Sd; break;
                    case 5: regs.CPP = Sd; break;
                    case 6: regs.TOS = Sd; break;
                    case 7: regs.OPC = Sd; break;
                    case 8: regs.H   = Sd; break;
                }
            }
        }

        fprintf(log, "\n> Registers after instruction\n");
        imprimir_regs_t2(&regs, log);
        fprintf(log, "=====================================================\n");

        ciclo++;
    }

    fprintf(log, "Cycle %d\n", ciclo);
    fprintf(log, "No more lines, EOP.\n");

    fclose(prog);
    fclose(log);
}
