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

/* EXERCICE 2 */

// Q 2.1

Segment *create_seg(int start, int size, Segment *next)
{
    /* Fonction qui cree un Segment */
    if (size <= 0)
        return NULL; // segment vide
    Segment *res = (Segment *)malloc(sizeof(Segment));
    if (!res)
        return NULL;
    res->start = start;
    res->size = size;
    res->next = next;
    return res;
}

MemoryHandler *memory_init(int size)
{
    /* Fonction permettant d’initialiser le gestionnaire de m´emoire */
    MemoryHandler *memory = (MemoryHandler *)malloc(sizeof(MemoryHandler));
    if (!memory)
        return NULL;
    // Le tableau de mémoire (pointeurs) est initialisé à NULL (non utilisé pour l’instant)
    memory->memory = NULL;
    memory->total_size = size;
    memory->free_list = create_seg(0, size, NULL);
    if (!memory->free_list)
    {
        free(memory);
        return NULL;
    }
    memory->allocated = hashmap_create();
    if (!memory->allocated)
    {
        free(memory->free_list);
        free(memory);
        return NULL;
    }
    return memory;
}

// Q 2.2

Segment *find_free_segment(MemoryHandler *handler, int start, int size, Segment **prev)
{
    /* Fonction qui retourne un tel segment libre ou NULL s'il n y a pas */

    if (handler == NULL || handler->free_list == NULL)
        return NULL;

    *prev = NULL;
    Segment *courr = handler->free_list;
    while (courr)
    {
        if (courr->start <= start && courr->start + courr->size >= start + size)
            // On a trouvé un Segment libre
            return courr;

        *prev = courr;
        courr = courr->next;
    }
    // Aucun segment trouvé → prev doit rester NULL
    *prev = NULL;
    return NULL;
}

// Q 2.3
int create_segment(MemoryHandler *handler, const char *name, int start, int size)
{
    if (!handler || !handler->free_list || !name || size <= 0)
        return 0;

    Segment *prev = NULL;
    Segment *seg_trouver = find_free_segment(handler, start, size, &prev);
    if (!seg_trouver)
        return 0;

    Segment *new_seg = create_seg(start, size, NULL);
    if (!new_seg)
        return 0;

    if (!hashmap_insert(handler->allocated, name, new_seg))
    {
        free(new_seg);
        return 0;
    }

    int old_start = seg_trouver->start;
    int old_end = seg_trouver->start + seg_trouver->size;
    int new_end = start + size;

    if (old_start == start && old_end == new_end)
    {
        // Cas 1 : segment entièrement pris
        if (prev)
            prev->next = seg_trouver->next;
        else
            handler->free_list = seg_trouver->next;

        free(seg_trouver);
    }
    else if (old_start == start)
    {
        // Cas 2 : juste la debut du segment est pris
        seg_trouver->start += size;
        seg_trouver->size -= size;
    }
    else if (old_end == new_end)
    {
        // Cas 3 : que le fin du segment est prise
        seg_trouver->size = start - old_start;
    }
    else
    {
        // Cas 4 : ons coupe au milieu
        Segment *after = create_seg(new_end, old_end - new_end, seg_trouver->next);
        seg_trouver->size = start - old_start;
        seg_trouver->next = after;
    }

    return 1;
}

// Q 2.4

int remove_segment(MemoryHandler *handler, const char *name)
{
    /* Fonction permettant de lib´erer un segment de m´emoire allou´e et de l’ajouter `a la liste des segments libres */
    if (!handler || !name)
        return 0;

    // recuperer le segment
    Segment *seg = hashmap_get(handler->allocated, name);
    if (!seg)
        return 0;

    // supprimer le segment de la table de hachage
    if (!hashmap_remove(handler->allocated, name))
        return 0;

    // inserer dans la liste des segments libres
    Segment *curr = handler->free_list;
    Segment *prev = NULL;

    while (curr && curr->start < seg->start)
    {
        prev = curr;
        curr = curr->next;
    }

    // inserer le segment a la bonne position
    seg->next = curr;
    if (prev)
        prev->next = seg;
    else
        handler->free_list = seg;

    // fusionner avec le segment suivant si adjacent
    if (curr && seg->start + seg->size == curr->start)
    {
        seg->size += curr->size;
        seg->next = curr->next;
        free(curr);
    }

    // fusionner avec le segment precedent si adjacent
    if (prev && prev->start + prev->size == seg->start)
    {
        prev->size += seg->size;
        prev->next = seg->next;
        free(seg);
    }

    return 1;
}
