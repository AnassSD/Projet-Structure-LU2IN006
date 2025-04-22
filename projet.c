#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "projet.h"

/* EXERCICE 1 */

// Q 1.1
unsigned long simple_hash(const char *str)
{
    /* Fonction permettant de convertir une chaine de caractères en un indice dans la table de hachage, avec une fonction de hachage simple */

    if (str == NULL)
        return 0;

    unsigned long ind = 0;
    while (*str != '\0')
    {
        // Ajoute la valeur ASCII du caractère courant à l’indice
        ind += (unsigned char)*str;
        str++;
    }
    return ind % TABLE_SIZE;
}

// Q 1.2
HashMap *hashmap_create()
{
    /* Fonction permettant d’allouer dynamiquement une table de hachage et d’initialiser ses cases à NULL */

    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL)
        return NULL;

    map->size = TABLE_SIZE;
    map->table = (HashEntry *)malloc(sizeof(HashEntry) * TABLE_SIZE);

    if (map->table == NULL)
    {
        free(map);
        return NULL;
    }

    // Initialiser chaque entrée de la table avec des clés et valeurs NULL
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        map->table[i].key = NULL;
        map->table[i].value = NULL;
    }

    return map;
}

// Q 1.3

int hashmap_insert(HashMap *map, const char *key, void *value)
{
    /* Fonction permettant d’ins´erer un ´el´ement dans la table de hachage, et de mettre à jour le valeur si le clé existe déjà */

    if (map == NULL || key == NULL)
        return 0;

    unsigned long index = simple_hash(key);
    for (int i = 0; i < map->size; i++)
    {
        int idx = (index + i) % map->size;

        // Mise à jour si la clé existe déjà
        if (map->table[idx].key != NULL && map->table[idx].key != TOMBSTONE &&
            strcmp(map->table[idx].key, key) == 0)
        {
            map->table[idx].value = value;
            return 1;
        }

        // Insertion quand on trouve une case vide ou supprimée
        if (map->table[idx].key == NULL || map->table[idx].key == TOMBSTONE)
        {
            map->table[idx].key = strdup(key); // copier la clé
            map->table[idx].value = value;
            return 1;
        }
    }

    return 0; // table pleine
}

// Q 1.4

void *hashmap_get(HashMap *map, const char *key)
{
    /* Fonction permettant de r´ecup´erer un ´el´ement `a partir de sa clé */

    if (map == NULL || key == NULL || map->table == NULL)
        return NULL;

    unsigned long ind = simple_hash(key);

    // Parcours de la table en sondage linéaire
    for (int i = 0; i < map->size; i++)
    {
        int idx = (ind + i) % map->size;
        // la clé n’existe pas dans la table
        if (map->table[idx].key == NULL)
            return NULL;

        if (map->table[idx].key != TOMBSTONE && strcmp(map->table[idx].key, key) == 0)
            return map->table[idx].value;
    }
    // Clé pas trouvé après parcours du tableau
    return NULL;
}

// Q 1.5

int hashmap_remove(HashMap *map, const char *key)
{
    /* Fonction permettant de supprimer un ´el´ement de la table de hachage tout en assurant la continuit´e du sondage lin´eaire. */
    if (map == NULL || key == NULL || map->table == NULL)
        return 0;

    unsigned long ind = simple_hash(key);

    for (int i = 0; i < map->size; i++)
    {
        int idx = (ind + i) % map->size;
        // la clé n’existe pas dans la table
        if (map->table[idx].key == NULL)
            return 0;

        if (map->table[idx].key != TOMBSTONE && strcmp(map->table[idx].key, key) == 0)
        {
            free(map->table[idx].key);
            map->table[idx].key = TOMBSTONE;
            map->table[idx].value = NULL;
            return 1;
        }
    }
    // Clé non trouvée
    return 0;
}

// Q 1.6

void hashmap_destroy(HashMap *map)
{
    /* Fonction permettant de lib´erer toute la m´emoire allou´ee `a la table de hachage*/

    if (map)
    {
        if (map->table)
        {
            for (int i = 0; i < map->size; i++)
            {
                if (map->table[i].key != NULL && map->table[i].key != TOMBSTONE)
                {
                    free((map->table[i]).key);
                }
            }
            free(map->table);
        }
        free(map);
    }
}
