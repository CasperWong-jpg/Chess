/**************************************************************************/
/*              COPYRIGHT Carnegie Mellon University 2021                 */
/* Do not post this file or any derivative on a public site or repository */
/**************************************************************************/
/*
 * Hash tables (dynamically size adjusting)
 *
 * 15-122 Principles of Imperative Computation
 */

#include "hdict.h"

/******************************/
/*** Library implementation ***/
/******************************/

typedef struct chain_node chain;

struct chain_node {
  entry data;       // != NULL; contains both key and value
  chain* next;
};

int chain_length(chain* p) {
  int i = 0;
  while (p != NULL) {
    i++;
    p = p->next;
  }
  return i;
}

typedef struct hdict_header hdict;
struct hdict_header {
  int size;             // size >= 0
  int capacity;         // capacity > 0
  chain **table;        // \length(table) == capacity
  entry_key_fn* key;    // != NULL
  key_hash_fn* hash;    // != NULL
  key_equiv_fn* equiv;  // != NULL
};

bool is_hdict(hdict* H) {
  if (H == NULL)          return false;
  if (!(H->size >= 0))    return false;
  if (!(H->capacity > 0)) return false;
  if (H->key   == NULL)   return false;
  if (H->hash  == NULL)   return false;
  if (H->equiv == NULL)   return false;

  //@assert \length(H->table) == H->capacity;
  /* check that each element of table is a valid chain */
  /* includes checking that all elements are non-null */
  return true;
}

int index_of_key(hdict* H, key k)
//@requires is_hdict(H) && k != NULL;
//@ensures 0 <= \result && \result < H->capacity;
{
  return abs((*H->hash)(k) % H->capacity);
}

key entry_key(hdict* H, entry x)
//@requires is_hdict(H) && x != NULL;
//@ensures \result != NULL;
{
  return (*H->key)(x);
}

bool key_equiv(hdict* H, key k1, key k2)
//@requires is_hdict(H) && k1 != NULL && k2 != NULL;
{
  return (*H->equiv)(k1, k2);
}

hdict* hdict_new(int capacity,
             entry_key_fn *entry_key, key_hash_fn* hash, key_equiv_fn* equiv)
//@requires capacity > 0;
//@requires entry_key != NULL && hash != NULL && equiv != NULL;
//@ensures is_hdict(\result);
{
  hdict* H = malloc(sizeof(hdict));
  H->size = 0;
  H->capacity = capacity;
  H->table = calloc(sizeof(chain*), capacity); /* initialized to NULL */
  H->key   = entry_key;
  H->hash  = hash;
  H->equiv = equiv;

  return H;
}

void hdict_free(hdict *H) {
    for (int i = 0; i < H->capacity; i++) {
        chain *node = H->table[i];
        while (node != NULL) {
            chain *tmp = node->next;
            free(node);
        }
    }
    free(H);
    return;
}

int hdict_size(hdict* H)
//@requires is_hdict(H);
{
  return H->size;
}

/* hdict_lookup(H, k) returns NULL if key k not present in H */
entry hdict_lookup(hdict* H, key k)
//@requires is_hdict(H) && k != NULL;
//@ensures \result == NULL || key_equiv(H, entry_key(H, \result), k);
{
  int i = index_of_key(H, k);
  chain* p = H->table[i];
  while (p != NULL)
    // loop invariant: p points to chain
    {
    //@assert p->data != NULL;
    if (key_equiv(H, entry_key(H, p->data), k))
      return p->data;
    else
      p = p->next;
  }
  /* not in chain */
  return NULL;
}

void hdict_resize(hdict* H, int new_capacity)
//@requires is_hdict(H);
//@requires new_capacity > 0;
//@ensures is_hdict(H);
//@ensures H->capacity == new_capacity;
{
  printf("(resizing ht to capacity %d\n", new_capacity);
  chain** old_table = H->table;
  chain** new_table = calloc(sizeof(chain*), new_capacity);
  /* new_table is initialized to all NULL */
  for (int i = 0; i < H->capacity; i++)
    //@loop_invariant 0 <= i;
    {
      chain* p = old_table[i];
      while (p != NULL) {
        chain* q = p->next;   /* save p->next */
        entry e = p->data;
        //@assert e != NULL;
        int h = abs((*H->hash)((*H->key)(e)) % new_capacity);
        p->next = new_table[h];
        new_table[h] = p;
        p = q;
      }
    }
  /* change ht H in place */
  /* H->size remains unchanged */
  H->capacity = new_capacity;
  H->table = new_table;
  return;
}

void hdict_insert(hdict* H, entry e)
//@requires is_hdict(H);
//@requires e != NULL;
//@ensures is_hdict(H);
//@ensures e == hdict_lookup(H, entry_key(H, e));
{
  key k = entry_key(H, e);
  int i = index_of_key(H, k);

  chain* p = H->table[i];
  while (p != NULL)
  // loop invariant: p points to chain
  {
    //@assert p->data != NULL;
    if (key_equiv(H, entry_key(H, p->data), k)) {
      /* overwrite existing element */
      p->data = e;
      return;
    } else {
      p = p->next;
    }
  }
  //@assert p == NULL;

  /* prepend new element */
  chain* q = malloc(sizeof(chain));
  q->data = e;
  q->next = H->table[i];
  H->table[i] = q;
  (H->size)++;

  /* resize hash table if load factor would be > 1 */
  if (H->size > H->capacity && H->capacity < int_max()/2) {
    /* load factor > 1 */
    assert(H->capacity < int_max()/2);
    hdict_resize(H, 2*H->capacity);
  }

  return;
}

// Debugging: report the distribution about the hashtable
void hdict_stats(hdict* H)
//@requires is_hdict(H);
{
  int max = 0;
  int *A = malloc(sizeof(int), 11);
  for(int i = 0; i < H->capacity; i++) {
    int j = chain_length(H->table[i]);
    if (j > 9) A[10]++;
    else A[j]++;
    if (j > max) max = j;
  }

  printf("Hash table distribution: how many chains have size...\n");
  for(int i = 0; i < 10; i++) {
    printf("...%d:   %d\n", i, A[i]);
  }
  printf("...10+: %d\n", A[10]);
  printf("Longest chain: %d\n", max);
}

void hdict_print(hdict* H,
                 key_print_fn* print_key, entry_print_fn* print_entry)
//@requires is_hdict(H);
//@requires print_key != NULL && print_entry != NULL;
{
  for(int i = 0; i < H->capacity; i++)
    for (chain* p = H->table[i]; p != NULL; p=p->next) {
      (*print_key)(entry_key(H, p->data));
      flush();
      printf(" => ");
      (*print_entry)(p->data);
      printf("\n");
    }
}