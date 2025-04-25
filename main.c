#include <stdio.h>
#include <stdlib.h>
#include "projet.h"

int main()
{
    const char *filename = "program.txt";

    // 1. Créer un programme assembleur d'exemple
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        perror("Erreur lors de la création du fichier");
        return EXIT_FAILURE;
    }

    fprintf(fp,
            ".DATA\n"
            "a DW 5\n"
            "b DB 10, 20\n"
            "\n"
            ".CODE\n"
            "MOV AX, a\n"
            "ADD AX, b\n"
            "CMP AX, 15\n"
            "JZ ok\n"
            "MOV BX, 0\n"
            "JMP end\n"
            "ok: MOV BX, 99\n"
            "end: HALT\n");
    fclose(fp);

    // 2. Analyser le fichier
    ParserResult *res = parse(filename);
    if (!res)
    {
        fprintf(stderr, "Erreur lors du parsing du fichier assembleur.\n");
        return EXIT_FAILURE;
    }

    // 3. Résoudre les constantes et labels
    if (resolve_constants(res) < 0)
    {
        fprintf(stderr, "Erreur lors de la résolution des constantes.\n");
        free_parser_result(res);
        return EXIT_FAILURE;
    }

    // 4. Initialiser le CPU
    CPU *cpu = cpu_init(64);
    if (!cpu)
    {
        fprintf(stderr, "Erreur d'initialisation du CPU.\n");
        free_parser_result(res);
        return EXIT_FAILURE;
    }

    // 5. Charger les données et instructions
    allocate_variables(cpu, res->data_instructions, res->data_count);
    allocate_code_segment(cpu, res->code_instructions, res->code_count);

    // 6. Exécuter le programme
    run_program(cpu);

    // 7. Nettoyer
    cpu_destroy(cpu);
    free_parser_result(res);

    return EXIT_SUCCESS;
}
