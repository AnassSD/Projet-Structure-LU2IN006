#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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
    memory->memory = calloc(size, sizeof(void *)); // Initialisé à NULL;
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

/* EXERCICE 3 */
Instruction *create_instruction(const char *mnemonic, const char *op1, const char *op2) {
    Instruction *inst = malloc(sizeof(Instruction));
    if (!inst) return NULL;
    inst->mnemonic = strdup(mnemonic);
    inst->operand1 = strdup(op1);
    inst->operand2 = strdup(op2);
    return inst;
}
// Q 3.1
Instruction *parse_data_instruction(const char *line, HashMap *memory_locations)
{
    /* Fonction qui permet d’analyser et stocker une ligne de la section .DATA d’un programme
    en pseudo-assembleur.*/

    // buffers
    char m[256], oper1[256], oper2[256];

    if (!line)
        return NULL;

    // Extraire les 3 composants
    if (sscanf(line, " %255s %255s %255[^\n]", m, oper1, oper2) < 2) // ici on a utilise %255[^\n] dans le cas ou l'input est avec des espaces entres le virgule par ex: " 5, 6"
        return NULL;

    Instruction *instruction = (Instruction *)malloc(sizeof(Instruction));
    if (!instruction)
        return NULL;

    instruction->mnemonic = strdup(m);
    instruction->operand1 = strdup(oper1);
    instruction->operand2 = strdup(oper2);

    // calculer la taille de la valeur 'oper2'
    int cmp = 1;
    for (char *p = oper2; *p; p++)
    {
        if (*p == ',')
            cmp++;
    }

    // Allouer et stocker l'adresse mémoire dans la table de hachage
    static int memory_address = 0;   // static pour l'incrementer d'un appel a un autre, elle garde la meoire entre les appels
    int *addr = malloc(sizeof(int)); // on stock un int* dans la table de hachage (le valeur)
    assert(addr);
    *addr = memory_address;
    hashmap_insert(memory_locations, strdup(m), addr);

    // Avancer le compteur mémoire
    memory_address += cmp;
    return instruction;
}

// Q 3.2

Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count)
{
    /* Fonction qui permet d’analyser et stocker une ligne de la section .CODE */

    if (!line || !labels)
        return NULL;

    // Buffers
    char label[256] = "", mnemonic[256] = "", operand1[256] = "", operand2[256] = "";
    Instruction *instruction = NULL;

    // Vérifie s'il y a une étiquette (label:) dans la ligne
    const char *colon = strchr(line, ':');

    if (colon)
    {
        // Ligne avec label
        sscanf(line, "%255s : %255s %255[^,] , %255[^\n]", label, mnemonic, operand1, operand2);

        // Insertion du label dans la table de hachage avec l'adresse code_count
        int *addr = malloc(sizeof(int));
        if (!addr)
            return NULL;
        *addr = code_count;
        hashmap_insert(labels, strdup(label), addr);

        // Création de l'instruction
        instruction = create_instruction(mnemonic, operand1, operand2);
    }
    else
    {
        // Ligne sans label
        int n = sscanf(line, "%255s %255[^,] , %255[^\n]", mnemonic, operand1, operand2);

        // On traite tous les cas
        if (n == 1)
            instruction = create_instruction(mnemonic, "", "");
        else if (n == 2)
            instruction = create_instruction(mnemonic, operand1, "");
        else
            instruction = create_instruction(mnemonic, operand1, operand2);
    }

    return instruction;
}

// Q 3.3
ParserResult *parse(const char *filename)
{
    /* Fonction qui analyse un fichier assembleur complet en identifiant les sections .DATA et .CODE et en traitant chaque ligne de la
    mani`ere appropri´ee.  */

    // on ouvre le fichier en read mode
    FILE *pf = fopen(filename, "r");
    if (!pf)
    {
        fprintf(stderr, "Le fichier est corrompue %s\n", filename);
        return NULL;
    }

    ParserResult *resultat = (ParserResult *)malloc(sizeof(ParserResult));
    if (!resultat)
    {
        fclose(pf);
        return NULL;
    }
    // initialisation des membre de ParserResult
    resultat->data_instructions = NULL;
    resultat->data_count = 0;
    resultat->code_instructions = NULL;
    resultat->code_count = 0;
    resultat->labels = hashmap_create();
    resultat->memory_locations = hashmap_create();

    // Vérification des allocations
    if (!resultat->labels || !resultat->memory_locations)
    {
        fclose(pf);
        free_parser_result(resultat);
        return NULL;
    }
    char line[256];

    BOOL dansData = 0, dansCode = 0;
    // Lecture du fichier ligne par ligne
    while (fgets(line, sizeof(line), pf))
    {
        // Détection des sections
        if (strcmp(line, ".DATA\n") == 0)
        {
            dansData = 1;
            dansCode = 0;
            continue;
        }
        else if (strcmp(line, ".CODE\n") == 0)
        {
            dansData = 0;
            dansCode = 1;
            continue;
        }
        if (dansData)
        {
            Instruction *instr = parse_data_instruction(line, resultat->memory_locations);
            if (instr)
            {
                resultat->data_count++;
                resultat->data_instructions = realloc(resultat->data_instructions, resultat->data_count * sizeof(Instruction *));
                resultat->data_instructions[resultat->data_count - 1] = instr;
            }
        }
        else if (dansCode)
        {
            Instruction *instr = parse_code_instruction(line, resultat->labels, resultat->code_count);
            if (instr)
            {
                resultat->code_count++;
                resultat->code_instructions = realloc(resultat->code_instructions, resultat->code_count * sizeof(Instruction *));
                resultat->code_instructions[resultat->code_count - 1] = instr;
            }
        }
    }

    fclose(pf);
    return resultat;
}

// Q 3.4
/* Les tests se trouvent dans le fichier test_ex3.c */
// on va ajouter qlq fonction pour simplifier les tests
void print_parser_result(ParserResult *result) {
    printf("DATA (%d):\n", result->data_count);
    for (int i = 0; i < result->data_count; ++i) {
        printf("  %s %s %s\n",
               result->data_instructions[i]->mnemonic,
               result->data_instructions[i]->operand1,
               result->data_instructions[i]->operand2);
    }

    printf("CODE (%d):\n", result->code_count);
    for (int i = 0; i < result->code_count; ++i) {
        printf("  %s %s %s\n",
               result->code_instructions[i]->mnemonic,
               result->code_instructions[i]->operand1,
               result->code_instructions[i]->operand2);
    }
}


// Q 3.5
void free_instruction(Instruction *instruction)
{
    /* Fonction permettant de suprrimer un element de type instruction */
    if (instruction)
    {
        free(instruction->mnemonic);
        free(instruction->operand1);
        free(instruction->operand2);
    }
}
void free_parser_result(ParserResult *result)
{
    /* Fonction permettant de supprimer un ´el´ement de type ParserResult. */
    if (result)
    {
        // Libérer le tableau d'instructions
        for (int i = 0; i < result->data_count; i++)
            free_instruction(result->data_instructions[i]);
        free(result->data_instructions); 

        // Libérer le tableau d'instructions
        for (int i = 0; i < result->code_count; i++)
            free_instruction(result->code_instructions[i]);
        free(result->code_instructions);

        hashmap_destroy(result->labels);
        hashmap_destroy(result->memory_locations);

        free(result);
    }

    // on a fait if avant pour eviter les problemes de free(NULL)
}