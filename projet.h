#ifndef PROJET_H
#define PROJET_H
#define TABLE_SIZE 128
#define TOMBSTONE ((void *)-1)

// struct de table de hachage

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

/* EXERCICE 1 */
unsigned long simple_hash(const char *str);
HashMap *hashmap_create();
int hashmap_insert(HashMap *map, const char *key, void *value);
void *hashmap_get(HashMap *map, const char *key);
int hashmap_remove(HashMap *map, const char *key);
void hashmap_destroy(HashMap *map);

/* EXERCICE 2 */
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

#endif