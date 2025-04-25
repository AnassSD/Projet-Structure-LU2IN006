#include <stdio.h>
#include <stdlib.h>
#include "projet.h" // Your full project is already here

int main() {
    const char *filename = "program.txt";

    // 1. Create a sample assembly program
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Erreur lors de la création du fichier");
        return 1;
    }

    // 2. Write valid .DATA and .CODE
    fprintf(fp,
        ".DATA\n"
        "a DW 5\n"
        "b DB 10, 20\n"
        ".CODE\n"
        "MOV AX, a\n"
        "ADD AX, b\n"
        "CMP AX, 15\n"
        "JZ ok\n"
        "MOV BX, 0\n"
        "JMP end\n"
        "ok: MOV BX, 99\n"
        "end: HALT\n"
    );
    fclose(fp);

    // 3. Parse the file
    ParserResult *res = parse(filename);
    if (!res) {
        fprintf(stderr, "Erreur de parsing\n");
        return 1;
    }

    // 4. Replace labels and variable names with memory addresses
    int replaced = resolve_constants(res);
    printf("Remplacements effectués : %d\n", replaced);

    // 5. Init CPU
    CPU *cpu = cpu_init(64);
    if (!cpu) {
        fprintf(stderr, "Erreur d'initialisation du CPU\n");
        free_parser_result(res);
        return 1;
    }

    // 6. Load memory and instructions
    allocate_variables(cpu, res->data_instructions, res->data_count);
    allocate_code_segment(cpu, res->code_instructions, res->code_count);

    // 7. Run the program
    run_program(cpu);

    // 8. Clean up
    cpu_destroy(cpu);
    free_parser_result(res);
    return 0;
}
