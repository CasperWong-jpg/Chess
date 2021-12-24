#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*************************/
/* client-side interface */
/*************************/

typedef void* entry;
typedef void* key;

typedef key entry_key_fn(entry e)            // Supplied by client
        /*@requires e != NULL; @*/
        /*@ensures \result != NULL; @*/ ;
typedef int key_hash_fn(key k)               // Supplied by client
        /*@requires k != NULL; @*/ ;
typedef bool key_equiv_fn(key k1, key k2)    // Supplied by client
        /*@requires k1 != NULL; @*/
        /*@requires k2 != NULL; @*/ ;
typedef void key_print_fn(key k)             // Supplied by client
        /*@requires k != NULL; @*/ ;
typedef void entry_print_fn(entry e)         // Supplied by client
        /*@requires e != NULL; @*/ ;

/**************************/
/* library side interface */
/**************************/

// Client-side type
typedef hdict* hdict_t;  // typedef ______* hdict_t;

hdict_t hdict_new(int capacity,
                  entry_key_fn* entry_key,
                  key_hash_fn* hash,
                  key_equiv_fn* equiv)
/*@requires capacity > 0; @*/
/*@requires entry_key != NULL && hash != NULL && equiv != NULL; @*/
/*@ensures \result != NULL; @*/ ;

entry hdict_lookup(hdict_t H, key k)             /* O(1) avg. */
/*@requires H != NULL && k != NULL; @*/ ;

void hdict_insert(hdict_t H, entry e)            /* O(1) avg. */
/*@requires H != NULL && e != NULL; @*/ ;

int hdict_size(hdict_t H)                        /* O(1) */
/*@requires H != NULL; @*/
/*@ensures \result >= 0; @*/ ;

void hdict_stats(hdict_t H)        /* Print debugging info about the table */
/*@requires H != NULL; @*/ ;

void hdict_print(hdict_t H,
                 key_print_fn* print_key, entry_print_fn* print_entry)
/*@requires H != NULL && print_key != NULL && print_entry != NULL; @*/ ;
