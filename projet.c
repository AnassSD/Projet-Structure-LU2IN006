#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <regex.h>
#include "projet.h"

/* ===================================================== EXERCICE 1 ===================================================== */

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
                    free(map->table[i].value); // ici on suppose que tous les valeurs sont allouées dynamiquement
                }
            }
            free(map->table);
        }
        free(map);
    }
}

/* ===================================================== EXERCICE 2 ===================================================== */

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

/* ===================================================== EXERCICE 3 ===================================================== */

Instruction *create_instruction(const char *mnemonic, const char *op1, const char *op2)
{
    /* Fonction qui cree une instruction */
    Instruction *inst = malloc(sizeof(Instruction));
    inst->mnemonic = strdup(mnemonic);
    inst->operand1 = op1 ? strdup(op1) : NULL;
    inst->operand2 = op2 ? strdup(op2) : NULL;
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
        // Extract label
        sscanf(line, " %255[^:]:", label);
        int *addr = malloc(sizeof(int));
        if (!addr) return NULL;
        *addr = code_count;
        hashmap_insert(labels, strdup(label), addr);

        // After the label, check if there is a mnemonic
        // Move pointer after label:
        const char *after_label = colon + 1;
        if (sscanf(after_label, " %255s %255[^,] , %255[^\n]", mnemonic, operand1, operand2) >= 1)
        {
            instruction = create_instruction(mnemonic, operand1, operand2);
        }
        else
        {
            // No instruction after label
            instruction = create_instruction("", "", "");
        }
    }
    else
    {
        // No label, normal instruction
        int n = sscanf(line, "%255s %255[^,] , %255[^\n]", mnemonic, operand1, operand2);
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
        char *clean = trim(line); // 🔥 remove leading/trailing whitespace

        if (strcmp(clean, "") == 0)
            continue; // skip empty lines

        if (strcmp(clean, ".DATA") == 0)
        {
            dansData = 1;
            dansCode = 0;
            continue;
        }
        else if (strcmp(clean, ".CODE") == 0)
        {
            dansData = 0;
            dansCode = 1;
            continue;
        }

        if (dansData)
        {
            Instruction *instr = parse_data_instruction(clean, resultat->memory_locations);
            if (instr  && instr->mnemonic && strcmp(instr->mnemonic, "") != 0 )
            {
                resultat->data_count++;
                resultat->data_instructions = realloc(resultat->data_instructions, resultat->data_count * sizeof(Instruction *));
                resultat->data_instructions[resultat->data_count - 1] = instr;
            }else {
                free(instr); // Don’t store NULL or empty instructions
            }

        }
        else if (dansCode)
        {
            Instruction *instr = parse_code_instruction(clean, resultat->labels, resultat->code_count);
            if (instr  && instr->mnemonic && strcmp(instr->mnemonic, "") != 0)
            {
                resultat->code_count++;
                resultat->code_instructions = realloc(resultat->code_instructions, resultat->code_count * sizeof(Instruction *));
                resultat->code_instructions[resultat->code_count - 1] = instr;
            }else {
                free(instr); // Don’t store NULL or empty instructions
            }
        }
    }

    fclose(pf);
    return resultat;
}

// Q 3.4
/* Les tests se trouvent dans le fichier test_ex3.c */
// on va ajouter qlq fonction pour simplifier les tests
void print_parser_result(ParserResult *result)
{
    printf("DATA (%d):\n", result->data_count);
    for (int i = 0; i < result->data_count; ++i)
    {
        printf("  %s %s %s\n",
               result->data_instructions[i]->mnemonic,
               result->data_instructions[i]->operand1,
               result->data_instructions[i]->operand2);
    }

    printf("CODE (%d):\n", result->code_count);
    for (int i = 0; i < result->code_count; ++i)
    {
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

/* ===================================================== EXERCICE 4 ===================================================== */

// Q 4.1

CPU *cpu_init(int memory_size)
{
    /* Fonction permettant d’initialiser le processeur simulee */

    if (memory_size <= 0)
        // CPU de memoire vide
        return NULL;

    // Creation des membres avec test d'allocation
    CPU *cpu = (CPU *)malloc(sizeof(CPU));
    if (!cpu)
        return NULL;

    cpu->memory_handler = memory_init(memory_size);
    if (!cpu->memory_handler)
    {
        free(cpu);
        return NULL;
    }
    cpu->context = hashmap_create();
    if (!cpu->context)
    {
        free(cpu->memory_handler);
        free(cpu);
        return NULL;
    }

    // Ajout des registres et initialisation à 0

    const char *regs[] = {"AX", "BX", "CX", "DX", "IP", "ZF", "SF"}; // modification 6.2
    for (int i = 0; i < 7; i++)                                      // modification 6.2
    {
        int *val = malloc(sizeof(int));

        // en cas d'erreur, on libère tout
        if (!val)
        {

            for (int j = 0; j < i; j++)
                free(hashmap_get(cpu->context, regs[j]));
            hashmap_destroy(cpu->context);
            free(cpu->memory_handler);
            free(cpu);
            return NULL;
        }
        *val = 0;
        hashmap_insert(cpu->context, regs[i], val);
    }

    // Q 5.1 Modification apres l'ajout de pool

    cpu->constant_pool = hashmap_create();
    // en cas d'erruer de malloc
    if (!cpu->constant_pool)
    {
        for (int i = 0; i < 7; i++) // modification 6.2
            free(hashmap_get(cpu->context, regs[i]));
        hashmap_destroy(cpu->context);
        free(cpu->memory_handler);
        free(cpu);
        return NULL;
    }

    return cpu;
}

// Q 4.2
void memory_handler_destroy(MemoryHandler *handler)
{
    if (handler)
    {
        // liberation de void **memory
        for (int i = 0; i < handler->total_size; i++)
            free(handler->memory[i]);
        free(handler->memory);

        // liberation de Segment *free_list
        Segment *courr = handler->free_list;
        while (courr)
        {
            Segment *next = courr->next;
            free(courr);
            courr = next;
        }

        // liberation de HashMap *allocated
        hashmap_destroy(handler->allocated);

        // liberation de handler
        free(handler);
    }
}
void cpu_destroy(CPU *cpu)
{
    /* Fonction permettant de liberer toutes les ressources allou´ees par le processeur simulee */
    if (cpu)
    {
        memory_handler_destroy(cpu->memory_handler);
        hashmap_destroy(cpu->context);
        // Q 5.1 modification
        hashmap_destroy(cpu->constant_pool); // Q 5.1
        free(cpu);
    }
}

// Q 4.3

void *store(MemoryHandler *handler, const char *segment_name, int pos, void *data)
{
    /* Fonction permettant de stocker une donnee data à la position pos de segment name. */

    if (!handler || !segment_name || pos < 0 || !data)
        return NULL;

    // On verifie si le segment est bien allouée alors il existe dans le table de hachage

    Segment *segment = hashmap_get(handler->allocated, segment_name);
    if (segment && segment->size > pos)
    {
        // le segment est allouee et pos ne depasse pas sa taille

        // Liberation d'ancienne valeur si elle existe
        if (handler->memory[segment->start + pos])
            free(handler->memory[segment->start + pos]);

        handler->memory[segment->start + pos] = data;
        return handler->memory[segment->start + pos];
    }
    return NULL;
}

// Q 4.4
void *load(MemoryHandler *handler, const char *segment_name, int pos)
{
    /* Fonction permettant de recuperer la donn´ee stock´ee `a la position pos de segment name. */
    if (handler && segment_name && pos >= 0)
    {
        Segment *segment = hashmap_get(handler->allocated, segment_name);
        if (segment && segment->size > pos)
        {
            return handler->memory[segment->start + pos];
        }
    }
    return NULL;
}

// Q 4.5

void allocate_variables(CPU *cpu, Instruction **data_instructions, int data_count)
{
    /* Fonction permettant d’allouer dynamiquement le segment de donn´ees en fonction des d´eclarations recuperees par le parser. */
    if (!cpu || !cpu->memory_handler || !data_instructions || data_count <= 0)
        return;

    int taille_totale = 0;

    // 1 calcul de la taille totale à allouer
    for (int i = 0; i < data_count; i++)
    {
        if (!data_instructions[i] || !data_instructions[i]->operand2)
            continue;

        int nb_valeurs = 1;
        for (char *p = data_instructions[i]->operand2; *p; p++)
        {
            if (*p == ',')
                nb_valeurs++;
        }

        taille_totale += nb_valeurs;
    }

    // 2 creation du segment DS
    create_segment(cpu->memory_handler, "DS", 0, taille_totale);

    // 3 Remplir la mémoire avec les valeurs
    int index = 0;
    for (int i = 0; i < data_count; i++)
    {
        if (!data_instructions[i] || !data_instructions[i]->operand2)
            continue;

        char *valeurs = strdup(data_instructions[i]->operand2);
        char *token = strtok(valeurs, ","); // pour prendre les valeurs separe par des virgules

        // NOTE ON SUPPOSE QUE LES VALEURS SONT PAS SEPARE PAR DES ESPACES

        while (token)
        {
            int *val = malloc(sizeof(int));
            *val = atoi(token);
            store(cpu->memory_handler, "DS", index, val);

            index++;
            token = strtok(NULL, ",");
        }

        free(valeurs);
    }
}

// Q 4.6

void print_data_segment(CPU *cpu)
{
    /* Fonction permettant d’afficher le contenu du segment de donnees */
    if (!cpu || !cpu->memory_handler)
    {
        printf("CPU non initialisé\n");
        return;
    }

    Segment *ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (!ds)
    {
        printf("Segment de données 'DS' introuvable.\n");
        return;
    }

    printf("Contenu du DS\n");

    for (int i = 0; i < ds->size; i++)
    {
        int addr = ds->start + i;
        void *ptr = cpu->memory_handler->memory[addr];

        if (ptr)
            printf("  [Adresse %d] = %d\n", addr, *(int *)ptr);
        else
            printf("  [Adresse %d] = vide\n", addr);
    }
}

/* ===================================================== EXERCICE 5 ===================================================== */

int matches(const char *pattern, const char *string)
{
    regex_t regex;
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result)
    {
        fprintf(stderr, "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }
    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

// Q 5.2

void *immediate_addressing(CPU *cpu, const char *operand)
{
    /* Fonction permettant de traiter l’adressage immediat */
    if (!cpu || !operand)
        return NULL;

    // si l’operande correspond `a ce mode d’adressage
    if (matches("^[0-9]+$", operand))
    {
        void *elem = hashmap_get(cpu->constant_pool, operand);
        if (!elem)
        {
            int *value = malloc(sizeof(int));
            // erreur de malloc
            if (!value)
                return NULL;

            *value = atoi(operand);
            hashmap_insert(cpu->constant_pool, strdup(operand), value);
            return value;
        }
        return elem;
    }
    return NULL;
}

// Q 5.3
void *register_addressing(CPU *cpu, const char *operand)
{
    /* Fonction permettant de trainter l'adressage oar registre */
    if (!cpu || !operand)
        return NULL;

    // si l’op´erande fourni correspond au format d’un nom de registre
    if (matches("^(AX|BX|CX|DX)$", operand))
        return hashmap_get(cpu->context, operand);

    return NULL;
}

// Q 5.4
void *memory_direct_addressing(CPU *cpu, const char *operand)
{
    /* Fonction permettant de traiter l’adressage direct par memoire */

    if (matches("^\\[[0-9]+\\]$", operand))
    {
        int pos = 0;
        if (sscanf(operand, "[%d]", &pos) != 1)
            return NULL;

        return load(cpu->memory_handler, "DS", pos);
    }
    return NULL;
}

// Q 5.5

void *register_indirect_addressing(CPU *cpu, const char *operand)
{
    /* Fonction permettant de traiter l’adressage indirect par registre */

    if (!cpu || !operand)
        return NULL;

    if (matches("^\\[(AX|BX|CX|DX)\\]$", operand))
    {
        char registre[4]; // 2 chars + il y a '\0' et un en plus pour eviter le risque
        if (sscanf(operand, "[%2s]", registre) != 1)
            return NULL;
        int *reg_value = (int *)hashmap_get(cpu->context, registre);
        // en cas d'erreur, ou le valeur de cette registre n'existe pas
        if (!reg_value)
            return NULL;

        int addr = *reg_value;
        return load(cpu->memory_handler, "DS", addr);
    }
    return NULL;
}

// Q 5.6
void handle_MOV(CPU *cpu, void *src, void *dest)
{
    /* Fonction permettant de simuler le comportement de l’instruction MOV en pseudo-assembleur */
    if (!cpu || !src || !dest)
        return;
    *(int *)dest = *(int *)src;
    // on a fait des cast car ils sont en void *, et *(int *) pour avoir le valeur de l'entier
    // ici on a pas fait des copie avec des malloc car c'est juste des int
    return;
}

// Q 5.7
/* Voir test_ex5.c */

// Q 5.8
void *resolve_addressing(CPU *cpu, const char *operand)
{
    /* Fonction permettant d’identifier automatiquement le mode d’adressage d’un op´erande et de r´esoudre sa valeur */
    if (!cpu || !operand)
        return NULL;

    void *res;

    res = immediate_addressing(cpu, operand);
    if (res)
    {

        return res;
    }
    res = register_addressing(cpu, operand);
    if (res)
    {
        // printf("Adressage par registre\n");
        return res;
    }
    res = memory_direct_addressing(cpu, operand);
    if (res)
    {
        /// printf("Adressage direct\n");
        return res;
    }
    res = register_indirect_addressing(cpu, operand);
    if (res)
    {
        // printf("Adressage indirect par registre\n");
        return res;
    }
    // printf("Mode inconnue\n");
    return NULL;
}

/* ===================================================== EXERCICE 6 ===================================================== */

char *trim(char *str)
{
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')
        str++;

    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r'))
    {
        *end = '\0';
        end--;
    }
    return str;
}

int search_and_replace(char **str, HashMap *values)
{
    int replaced = 0;
    char *input = *str;

    // Iterate through ALL keys in the hashmap
    for (int i = 0; i < values->size; i++)
    {
        if (values->table[i].key != NULL && values->table[i].key != (void *)-1)
        {
            char *key = values->table[i].key;
            int value = *(int *)(values->table[i].value);

            // Find potential substring match
            char *substr = strstr(input, key);
            if (substr)
            {
                // Construct replacement buffer
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "[%d]", value);

                // Calculate lengths
                int key_len = strlen(key);
                int repl_len = strlen(replacement);
                // int remain_len = strlen(substr + key_len);

                // Create new string
                char *new_str = (char *)malloc(strlen(input) - key_len + repl_len + 1);
                strncpy(new_str, input, substr - input);
                new_str[substr - input] = '\0';
                strcat(new_str, replacement);
                strcat(new_str, substr + key_len);

                // Free and update original string
                free(input);
                *str = new_str;
                input = new_str;

                replaced = 1;
            }
        }
    }

    // Trim the final string
    if (replaced)
    {
        char *trimmed = trim(input);
        if (trimmed != input)
        {
            memmove(input, trimmed, strlen(trimmed) + 1);
        }
    }

    return replaced;
}

// Q 6.1

int resolve_constants(ParserResult *result)
{
    /* Fonction qui remplace les variables par leur adresse dans le segment de donn´ees et les ´etiquettes par leur adresse dans le code.
        et retourne le nombre des remplacements */

    if (!result || !result->code_instructions)
        return -1;

    int nb_remplacement = 0;
    for (int i = 0; i < result->code_count; i++)
    {
        Instruction *code_instruct = result->code_instructions[i];

        if (code_instruct->operand1)
            nb_remplacement += search_and_replace(&code_instruct->operand1, result->memory_locations) + search_and_replace(&code_instruct->operand1, result->labels);
        if (code_instruct->operand2)
            nb_remplacement += search_and_replace(&code_instruct->operand2, result->memory_locations) + search_and_replace(&code_instruct->operand2, result->labels);

        // j'ai ajouté c'est pour le debugging
        printf("Instruction après remplacement: %s %s %s\n",
               code_instruct->mnemonic,
               code_instruct->operand1 ? code_instruct->operand1 : "",
               code_instruct->operand2 ? code_instruct->operand2 : "");
    }
    return nb_remplacement;
}

// Q 6.2

// Q 6.3

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count)
{
    /* Fonction qui alloue et initialise le segment de code (CS). */

    if (!cpu || !cpu->memory_handler || !code_instructions || code_count <= 0)
        return;

    // creation de segment CS
    if (!create_segment(cpu->memory_handler, "CS", 0, code_count))
        return;

    // stock les instructions dans le segment
    for (int i = 0; i < code_count; i++)
    {
        store(cpu->memory_handler, "CS", i, code_instructions[i]);
        // debugging print
        printf("Instruction stockée [%d] : %s %s %s\n", i,
               code_instructions[i]->mnemonic,
               code_instructions[i]->operand1 ? code_instructions[i]->operand1 : "",
               code_instructions[i]->operand2 ? code_instructions[i]->operand2 : "");
    }

    // init IP à 0
    int *ip = hashmap_get(cpu->context, "IP");
    if (ip)
        *ip = 0;
}

// Q 6.4

int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest)
{
    /* Fonction qui g´en´eralise la fonction handle MOV en permettant d’ex´ecuter une instruction
        dans le CPU en fonction de son mn´emonique. */

    if (!cpu || !instr || !instr->mnemonic)
        return 0;

    const char *mnemonic = instr->mnemonic;

    int *zf = hashmap_get(cpu->context, "ZF");
    int *sf = hashmap_get(cpu->context, "SF");
    int *ip = hashmap_get(cpu->context, "IP");

    // NOTE: On fait des cast avant car c'est void *

    // MOV
    if (strcmp(mnemonic, "MOV") == 0 && src && dest)
    {
        *(int *)dest = *(int *)src; // int pas besoin de malloc
    }
    // ADD
    else if (strcmp(mnemonic, "ADD") == 0 && src && dest)
    {
        *(int *)dest += *(int *)src;
    }
    // CMP
    else if (strcmp(mnemonic, "CMP") == 0 && src && dest)
    {
        int result = *(int *)dest - *(int *)src;

        if (result == 0)
            *zf = 1;
        else
            *zf = 0;

        if (result < 0)
            *sf = 1;
        else
            *sf = 0;
    }

    // JMP
    else if (strcmp(mnemonic, "JMP") == 0 && dest)
    {
        *ip = *(int *)dest - 1;
    }

    // JZ
    else if (strcmp(mnemonic, "JZ") == 0 && dest)
    {
        if (*zf == 1)
            *ip = *(int *)dest - 1;
    }

    // JNZ
    else if (strcmp(mnemonic, "JNZ") == 0 && dest)
    {
        if (*zf == 0)
            *ip = *(int *)dest - 1;
    }

    else
    {
        return 0; // mnemonic non reconnue
    }

    return 1;
}

// Q 6.5

int execute_instruction(CPU *cpu, Instruction *instr)
{
    /* Fonction  qui permet de r´esoudre les adresses des op´erandes en fonction du type d’adressage, puis d´el`egue l’ex´ecution
    proprement dite `a la fonction handle instruction.*/

    if (!cpu || !instr)
        return 0;

    void *dest = NULL;
    void *src = NULL;

    // resolve les adresse (deja fait)
    if (instr->operand1)
        dest = resolve_addressing(cpu, instr->operand1);

    if (instr->operand2)
        src = resolve_addressing(cpu, instr->operand2);

    return handle_instruction(cpu, instr, src, dest);
}

// Q 6.6
Instruction *fetch_next_instruction(CPU *cpu)
{
    /* Fonction qui recupere l’instruction suivante dans le segment de codes et incremente le pointeur d’instruction (IP) */
    if (!cpu)
        return NULL;

    int *ip = hashmap_get(cpu->context, "IP");
    if (!ip)
        return NULL;

    Segment *cs = hashmap_get(cpu->memory_handler->allocated, "CS");
    if (!cs)
        return NULL;

    if (*ip < 0 || *ip >= cs->size)
        // IP est pas dans les limites valides
        return NULL;

    // charger l'instruction
    Instruction *instr = (Instruction *)load(cpu->memory_handler, "CS", *ip);

    // incrementer registre IP
    (*ip)++;

    // debugg print

    printf("Instruction récupérée (IP=%d): %s %s %s\n", *ip - 1,
           instr ? instr->mnemonic : "NULL",
           instr && instr->operand1 ? instr->operand1 : "",
           instr && instr->operand2 ? instr->operand2 : "");
    if (!instr)
        printf("Instruction NULL à IP=%d\n", *ip);

    return instr;
}

// Q 6.7

int run_program(CPU *cpu)
{
    if (!cpu)
        return 0;

    int *ip = hashmap_get(cpu->context, "IP");
    if (!ip)
        return 0;

    printf("=== État initial du CPU ===\n");
    print_data_segment(cpu); // Affiche le segment .DATA
    // Affiche les registres
    printf("Registres :\n");
    printf("AX = %d\n", *(int *)hashmap_get(cpu->context, "AX"));
    printf("BX = %d\n", *(int *)hashmap_get(cpu->context, "BX"));
    printf("CX = %d\n", *(int *)hashmap_get(cpu->context, "CX"));
    printf("DX = %d\n", *(int *)hashmap_get(cpu->context, "DX"));
    printf("IP = %d\n", *ip);
    printf("ZF = %d\n", *(int *)hashmap_get(cpu->context, "ZF"));
    printf("SF = %d\n", *(int *)hashmap_get(cpu->context, "SF"));

    printf("\n--- Exécution en mode pas à pas ---\n");
    printf("Appuie sur 'Enter' pour exécuter l’instruction suivante, 'q' pour quitter\n\n");

    char input[10];
    while (1)
    {
        Instruction *instr = fetch_next_instruction(cpu);
        if (!instr || !instr->mnemonic)
        {
            printf("Fin du programme ou instruction invalide.\n");
            break;
        }

        printf("Instruction : %s %s %s\n",
               instr->mnemonic,
               instr->operand1 ? instr->operand1 : "",
               instr->operand2 ? instr->operand2 : "");

        printf(" -> Entrée pour continuer, 'q' pour quitter > ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == 'q')
        {
            printf("Exécution interrompue par l’utilisateur.\n");
            break;
        }

        execute_instruction(cpu, instr);
    }

    printf("\n=== État final du CPU ===\n");
    print_data_segment(cpu); // Affiche le segment .DATA à la fin
    printf("Registres :\n");
    printf("AX = %d\n", *(int *)hashmap_get(cpu->context, "AX"));
    printf("BX = %d\n", *(int *)hashmap_get(cpu->context, "BX"));
    printf("CX = %d\n", *(int *)hashmap_get(cpu->context, "CX"));
    printf("DX = %d\n", *(int *)hashmap_get(cpu->context, "DX"));
    printf("IP = %d\n", *(int *)hashmap_get(cpu->context, "IP"));
    printf("ZF = %d\n", *(int *)hashmap_get(cpu->context, "ZF"));
    printf("SF = %d\n", *(int *)hashmap_get(cpu->context, "SF"));

    return 1;
}
