#include <stdio.h>
#include <stdlib.h>
#include "projet.h"

int main() {
    printf("=== TEST D'EXERCICE 4 ===\n\n");

    // 1. Initialiser un CPU avec 32 cases mémoire
    CPU *cpu = cpu_init(32);
    if (!cpu) {
        printf("Erreur : impossible d'initialiser le CPU.\n");
        return 1;
    }

    // 2. Déclarer un tableau d'instructions .DATA (avec cas simples + cas limite)
    Instruction *data1 = create_instruction("X", "DW", "10");
    Instruction *data2 = create_instruction("Y", "DB", "1,2,3");
    Instruction *data3 = create_instruction("Z", "DW", "42");
    Instruction *data4 = create_instruction("Trop", "DB", "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18"); // cas limite

    Instruction *data_instructions[] = { data1, data2, data3, data4 };
    int data_count = sizeof(data_instructions) / sizeof(data_instructions[0]);

    // 3. Allouer dynamiquement les variables dans le segment "DS"
    allocate_variables(cpu, data_instructions, data_count);

    // 4. Afficher le contenu du segment "DS"
    print_data_segment(cpu);

    // 5. Tester le comportement de load() et store()
    printf("\n=== TEST LECTURE ET ÉCRITURE ===\n");

    int *valeur = load(cpu->memory_handler, "DS", 0);
    if (valeur)
        printf("Valeur à l'adresse 0 : %d\n", *valeur);
    else
        printf("Rien à l'adresse 0\n");

    int *nouvelle_val = malloc(sizeof(int));
    *nouvelle_val = 99;
    store(cpu->memory_handler, "DS", 1, nouvelle_val);
    printf("Nouvelle valeur 99 écrite à l'adresse 1\n");

    printf("Affichage après écriture :\n");
    print_data_segment(cpu);

    // 6. Tester un accès hors limites
    printf("\n=== TEST ACCÈS HORS LIMITES ===\n");
    void *erreur = load(cpu->memory_handler, "DS", 100);
    if (!erreur)
        printf("Accès à l'adresse 100 refusé (hors limites) \n");
    else
        printf("Erreur : accès hors limites autorisé \n");

    // 7. Libérer la mémoire
    cpu_destroy(cpu);
    free_instruction(data1);
    free_instruction(data2);
    free_instruction(data3);
    free_instruction(data4);

    printf("\n=== FIN DES TESTS ===\n");

    return 0;
}
