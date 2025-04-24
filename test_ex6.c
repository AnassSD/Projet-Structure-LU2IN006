#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "projet.h"  



void setup_sample_program(Instruction **instructions) {
    instructions[0] = create_instruction("MOV", "AX", "5");     // AX = 5
    instructions[1] = create_instruction("MOV", "BX", "7");     // BX = 7
    instructions[2] = create_instruction("ADD", "AX", "BX");    // AX = AX + BX → 12
    instructions[3] = create_instruction("CMP", "AX", "12");    // Compare AX == 12 → ZF = 1
    instructions[4] = create_instruction("JZ", "[6]", NULL);    // Si ZF == 1 → IP = 6
    instructions[5] = create_instruction("MOV", "CX", "0");     // Sauté si ZF == 1
    instructions[6] = create_instruction("MOV", "DX", "99");    // DX = 99

}

int main()
{
    // 1. Initialiser le CPU
    CPU *cpu = cpu_init(1024);

    // 2. Créer les instructions en mémoire
    int code_count = 7;
    Instruction *code[7];
    setup_sample_program(code);

    // 3. Allouer le segment de code
    allocate_code_segment(cpu, code, code_count);

    // 4. Exécuter le programme en mode pas-à-pas
    run_program(cpu);

    // 5. Nettoyage
    cpu_destroy(cpu);
    
    return 0;
}
