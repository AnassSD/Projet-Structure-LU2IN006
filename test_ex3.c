#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "projet.h"


void print_instruction(const char *section, int index, Instruction *instr) {
    printf("%s [%02d] %s %s %s\n", section, index,
           instr->mnemonic ? instr->mnemonic : "(null)",
           instr->operand1 ? instr->operand1 : "",
           instr->operand2 ? instr->operand2 : "");
}

void print_hashmap(const char *title, HashMap *map) {
    printf("\n-- %s --\n", title);
    if (!map) {
        printf("(vide)\n");
        return;
    }
    for (int i = 0; i < map->size; ++i) {
        if (map->table[i].key && map->table[i].key != TOMBSTONE) {
            int *val = (int *)map->table[i].value;
            printf("  %s → %d\n", map->table[i].key, val ? *val : -1);
        }
    }
}

int main() {
    const char *filename = "test_program.asm";
    FILE *fp = fopen(filename, "w");

    if (!fp) {
        perror("Erreur lors de la création du fichier test");
        return 1;
    }

    // Programme assembleur avec un peu de tout
    fprintf(fp,
        ".DATA\n"
        "x DW 1\n"
        "y DB 5,6\n"
        "tab DB 10, 11, 12\n"
        "mauvaise\n"
        "\n"
        ".CODE\n"
        "start: MOV AX , x\n"
        "ADD AX , y\n"
        "JMP start\n"
        "JZ 2\n"
        "HALT\n"
        "erreur: MOV AX\n"
        "start: NOP\n" // doublon volontaire
    );
    fclose(fp);

    ParserResult *result = parse(filename);
    if (!result) {
        printf(" Parsing échoué\n");
        return 1;
    }

    printf("========== Résultat du parsing ==========\n");

    printf("\nSECTION .DATA (%d instructions)\n", result->data_count);
    for (int i = 0; i < result->data_count; i++) {
        print_instruction(".DATA", i, result->data_instructions[i]);
    }

    printf("\nSECTION .CODE (%d instructions)\n", result->code_count);
    for (int i = 0; i < result->code_count; i++) {
        print_instruction(".CODE", i, result->code_instructions[i]);
    }

    print_hashmap("Adresses mémoire des variables (memory_locations)", result->memory_locations);
    print_hashmap("Étiquettes (labels)", result->labels);

    printf("\n Test terminé. Mémoire libérée.\n");

    free_parser_result(result);
    remove(filename);

    return 0;
}
