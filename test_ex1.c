#include <stdio.h>
#include <stdlib.h>
#include "projet.h"

int main() {
    printf("Test d'EXERCICE 1 \n=== Début des tests de la table de hachage ===\n");

    // Création de la table
    HashMap *map = hashmap_create();
    if (!map) {
        printf("Erreur lors de la création de la table.\n");
        return 1;
    }

    // Insertion de quelques valeurs
    int a = 10, b = 20, c = 30;
    hashmap_insert(map, "cléA", &a);
    hashmap_insert(map, "cléB", &b);
    hashmap_insert(map, "cléC", &c);

    // Récupération de valeurs
    int *pa = (int *)hashmap_get(map, "cléA");
    if (pa) printf("cléA → %d (ok)\n", *pa);
    else printf("cléA non trouvée\n");

    int *pb = (int *)hashmap_get(map, "cléB");
    if (pb) printf("cléB → %d (ok)\n", *pb);

    // Clé absente
    int *px = (int *)hashmap_get(map, "cléX");
    if (px == NULL) printf("cléX absente (normal)\n");

    // Suppression
    if (hashmap_remove(map, "cléB")) {
        printf("cléB supprimée\n");
    } else {
        printf("échec suppression cléB\n");
    }

    // Vérification après suppression
    pb = (int *)hashmap_get(map, "cléB");
    if (pb == NULL) {
        printf("cléB bien supprimée (ok)\n");
    }

    // Réinsertion dans une case supprimée
    int d = 999;
    hashmap_insert(map, "cléB", &d);
    pb = (int *)hashmap_get(map, "cléB");
    if (pb) printf("cléB (nouvelle) → %d (ok)\n", *pb);

    // quelque cas spécifiques
    hashmap_insert(map, NULL, &a);
    hashmap_insert(NULL, "test", &a);
    hashmap_get(map, NULL);
    hashmap_remove(map, NULL);

    // Destruction de la table
    hashmap_destroy(map);
    //ici on peut verifier avec Valgrind 

    return 0;
}
