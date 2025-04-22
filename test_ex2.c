#include <stdio.h>
#include <stdlib.h>
#include "projet.h"

// Petite fonction pour afficher les segments libres
void afficher_free_list(MemoryHandler *handler) {
    printf("Segments libres :\n");
    Segment *s = handler->free_list;
    while (s) {
        printf(" start: %d, size: %d\n", s->start, s->size);
        s = s->next;
    }
}

int main() {
    printf("=== TEST DE L’EXERCICE 2 ===\n");

    // Initialisation
    MemoryHandler *handler = memory_init(100);
    if (!handler) {
        printf("Erreur d'initialisation.\n");
        return 1;
    }
    printf("Mémoire initialisée.\n");
    afficher_free_list(handler);
    printf("\n");

    // Création de segments
    create_segment(handler, "a", 0, 20);
    create_segment(handler, "b", 20, 30);
    create_segment(handler, "c", 50, 30);  // devrait fonctionner

    printf("\nAprès création de a, b et c :\n");
    afficher_free_list(handler);

    // Suppression
    remove_segment(handler, "b");
    printf("\nAprès suppression de b :\n");
    afficher_free_list(handler);

    // Création d’un segment qui ne rentre pas
    int ok = create_segment(handler, "d", 85, 20);  // trop grand
    if (!ok)
        printf("\nCréation de 'd' a échoué comme prévu (trop grand).\n");

    // Suppression d’un segment inexistant
    int del = remove_segment(handler, "x");  // n’existe pas
    if (!del)
        printf("Suppression de 'x' a échoué (normal).\n");

    // Nettoyage
    hashmap_destroy(handler->allocated);
    free(handler->free_list);
    free(handler);

    printf("\nFin des tests.\n");
    return 0;
}
