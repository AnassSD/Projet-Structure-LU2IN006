#ifndef PROJET_H
#define PROJET_H
#define TABLE_SIZE 128
#define TOMBSTONE ((void *)-1)
typedef int BOOL;

/* ===================================================== EXERCICE 1 ===================================================== */

typedef struct hashentry
{
    char *key;
    void *value;
} HashEntry;

typedef struct hashmap
{
    int size;
    HashEntry *table;
} HashMap;

unsigned long simple_hash(const char *str);
HashMap *hashmap_create();
int hashmap_insert(HashMap *map, const char *key, void *value);
void *hashmap_get(HashMap *map, const char *key);
int hashmap_remove(HashMap *map, const char *key);
void hashmap_destroy(HashMap *map);

/* ===================================================== EXERCICE 2 ===================================================== */
typedef struct segment
{
    int start;            // Position de debut (adresse) du segment dans la memoire
    int size;             // Taille du segment en unites de memoire
    struct segment *next; // Pointeur vers le segment suivant dans la liste chainee
} Segment;

typedef struct memoryHandler
{
    void **memory;      // Tableau de pointeurs vers la memoire allouee
    int total_size;     // Taille totale de la memoire geree
    Segment *free_list; // Liste chainee des segments de memoire libres
    HashMap *allocated; // Table de hachage (nom, segment)
} MemoryHandler;

MemoryHandler *memory_init(int size);
Segment *find_free_segment(MemoryHandler *handler, int start, int size, Segment **prev);
int create_segment(MemoryHandler *handler, const char *name, int start, int size);
int remove_segment(MemoryHandler *handler, const char *name);
void memory_handler_destroy(MemoryHandler *handler);

/* ===================================================== EXERCICE 3 ===================================================== */
typedef struct
{
    char *mnemonic;
    char *operand1;
    char *operand2;
} Instruction;

typedef struct
{
    Instruction **data_instructions; // Tableau d’instructions .DATA
    int data_count;                  // Nombre d’instructions .DATA
    Instruction **code_instructions; // Tableau d’instructions .CODE
    int code_count;                  // Nombre d’instructions .CODE
    HashMap *labels;                 // labels -> indices dans code_instructions
    HashMap *memory_locations;       // noms de variables -> adresse memoire
} ParserResult;

Instruction *create_instruction(const char *mnemonic, const char *op1, const char *op2);
Instruction *parse_data_instruction(const char *line, HashMap *memory_locations);
Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count);
ParserResult *parse(const char *filename);
void print_parser_result(ParserResult *result);
void free_instruction(Instruction *instruction);
void free_parser_result(ParserResult *result);

/* ===================================================== EXERCICE 4 ===================================================== */
typedef struct
{
    MemoryHandler *memory_handler; // Gestionnaire de memoire
    HashMap *context;              // Registres (AX, BX, CX, DX)
    HashMap *constant_pool;        // Table de hachage pour stocker les valeurs i m m d i a t e s
} CPU;

CPU *cpu_init(int memory_size);
void cpu_destroy(CPU *cpu);
void *store(MemoryHandler *handler, const char *segment_name, int pos, void *data);
void *load(MemoryHandler *handler, const char *segment_name, int pos);
void allocate_variables(CPU *cpu, Instruction **data_instructions, int data_count);
void print_data_segment(CPU *cpu);

/* ===================================================== EXERCICE 5 ===================================================== */

int matches(const char *pattern, const char *string);
void *immediate_addressing(CPU *cpu, const char *operand);
void *register_addressing(CPU *cpu, const char *operand);
void *memory_direct_addressing(CPU *cpu, const char *operand);
void *register_indirect_addressing(CPU *cpu, const char *operand);
void handle_MOV(CPU *cpu, void *src, void *dest);
void *resolve_addressing(CPU *cpu, const char *operand);

#endif