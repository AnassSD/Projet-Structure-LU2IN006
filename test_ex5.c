#include <stdio.h>
#include <stdlib.h>
#include "projet.h"

CPU *setup_test_environment()
{
    // Initialiser le CPU
    CPU *cpu = cpu_init(1024);
    if (!cpu)
    {
        printf("Erreur : CPU initialisation failed\n");
        return NULL;
    }

    // Initialiser les registres avec des valeurs spécifiques
    int *ax = (int *)hashmap_get(cpu->context, "AX");
    int *bx = (int *)hashmap_get(cpu->context, "BX");
    int *cx = (int *)hashmap_get(cpu->context, "CX");
    int *dx = (int *)hashmap_get(cpu->context, "DX");

    *ax = 3;
    *bx = 6;
    *cx = 100;
    *dx = 0;

    // Créer et initialiser le segment de données
    if (!hashmap_get(cpu->memory_handler->allocated, "DS"))
    {
        create_segment(cpu->memory_handler, "DS", 0, 20);

        // Initialiser le segment de données avec des valeurs de test
        for (int i = 0; i < 10; i++)
        {
            int *value = (int *)malloc(sizeof(int));
            *value = i * 10 + 5; // Valeurs 5, 15, 25, 35...
            store(cpu->memory_handler, "DS", i, value);
        }
    }

    printf("Test environment initialized.\n\n");
    return cpu;
}

int main()
{
    CPU *cpu = setup_test_environment();
    if (!cpu)
        return 1;

    // MOV AX, 42 (immediate)
    void *src1 = resolve_addressing(cpu, "42");
    void *dest1 = resolve_addressing(cpu, "AX");
    handle_MOV(cpu, src1, dest1);
    printf("AX après MOV 42 : %d\n\n", *(int *)dest1);

    // MOV CX, BX (register)
    void *src2 = resolve_addressing(cpu, "BX");
    void *dest2 = resolve_addressing(cpu, "CX");
    handle_MOV(cpu, src2, dest2);
    printf("CX après MOV BX : %d\n\n", *(int *)dest2);

    // MOV DX, [5] (direct memory)
    void *src3 = resolve_addressing(cpu, "[5]");
    void *dest3 = resolve_addressing(cpu, "DX");
    handle_MOV(cpu, src3, dest3);
    printf("DX après MOV [5] : %d\n\n", *(int *)dest3);

    // MOV BX, [AX] (indirect memory)
    void *src4 = resolve_addressing(cpu, "[AX]");
    void *dest4 = resolve_addressing(cpu, "BX");
    handle_MOV(cpu, src4, dest4);
    printf("BX après MOV [AX] : %d\n\n", *(int *)dest4);
    
    // on libere
    cpu_destroy(cpu);
    return 0;
}
