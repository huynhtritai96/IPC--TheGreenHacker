#include <stdio.h>
#include <stdlib.h>
#include "dll.h"

/* Initialize an empty list in which the head and the tail are the same, i.e. they both point
 to a dummy node. */
 /*
dll_t
+------------------+
| head ─────────┐  |
| tail ─────────┘  |
+------------------+
                  v
            +------------------+
            |  SENTINEL NODE   |
            |------------------|
            | prev ──┐         |
            | next ──┘         |
            +------------------+
                ^        |
                |________|

 */
dll_t *init_dll() {
    dll_t *dll = calloc(1, sizeof(dll_t)); // <-- main DLL structure
    dll_node_t *node = calloc(1, sizeof(dll_node_t)); // <-- dummy sentinel node

    // No data for sentinel node
    node->next = node; // <-- point to itself
    node->prev = node;

    dll->head = node; // empty list consists of only the dummy node
    dll->tail = node;
    return dll;
}

/* Add new node with data to back of list. */
/*
dll_t
+------------------+
| head ─────────┐  |
| tail ───────┐ |  |
+-------------|-|--+
              | |
              v v
        +-----------+      +-----------+
        | SENTINEL  | <--> |   NODE    |
        +-----------+      +-----------+
        | prev ──┐  |      | prev ──┐  |
        | next ──┘  |      | next ──┘  |
        +-----------+      +-----------+
               ^              |
               |______________| 


-------------------------------------------------------------------------------
After Multiple Appends
dll_t
                        +------------------+
                        | tail ─────────┐  |
                        |               |  |
                        +---------------|--+
                                        |
                                        v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->  [N1]  <->  [N2]   [N3]


-------------------------------------------------------------------------------
After Multiple Appends
dll_t
                        +------------------+
                        | tail ─────────┐  |
                        |               |  |
                        +---------------|--+
                                        |
                                        v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->  [N1]  <->  [N2] <--  [N3] <--- node->next = dll->tail;
                ^                                   |
                |___________________________________| <--- node->next = dll->head;


-------------------------------------------------------------------------------
After Multiple Appends
                dll_t
                                    +------------------+
                                    | tail ─────────┐  |
                                    |               |  |
                                    +---------------|--+
                                                    |
                                                    v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->  [N1]  <->  [N2] <->  [N3] <-- dll->tail->next = node;
                ^                                   ^ <-- dll->head->prev = node; 
                |___________________________________|

*/
void append(dll_t *dll, void *data) {
    dll_node_t *head = dll->head; 
    dll_node_t *node = calloc(1, sizeof(dll_node_t));

    node->data = data; // <-- set data
    node->next = dll->head;
    node->prev = dll->tail;

    dll->head->prev = node; // <-- link head's prev to new node
    dll->tail->next = node;
    dll->tail = node;

    if (dll->head != head) {
        printf("Fuck something's wrong...\n");
    }
}

/*
/* Delete a node from the list. */
/*
-------------------------------------------------------------------------------
Initial State (node to delete is in the middle)

                                            dll_t
                                            +------------------+
                                            | tail ─────────┐  |
                                            |               |  |
                                            +---------------|--+
                                                            |
                                                            v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->   [N1]   <->   [NODE]  <->   [N3]
                ^                                          |
                |__________________________________________|

-------------------------------------------------------------------------------
Step 1: Forward link bypass
node->prev->next = node->next;

            [SENTINEL]  <->   [N1]   ------     ------>   [N3]
                                            X
                                            X
                                          [NODE]
                ^                                          |
                |__________________________________________|

-------------------------------------------------------------------------------
Step 2: Backward link bypass
node->next->prev = node->prev;

            [SENTINEL]  <->   [N1]   <-----     ------   [N3]
                                            X
                                            X
                                          [NODE]
                ^                                          |
                |__________________________________________|

-------------------------------------------------------------------------------
After Step 1 + Step 2 (NODE fully unlinked)

                dll_t
                            +------------------+
                            | tail ─────────┐  |
                            |               |  |
                            +---------------|--+
                                            |
                                            v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]   <->   [N1]   <->   [N3]
                ^                             |
                |_____________________________|

-------------------------------------------------------------------------------
Step 3: Tail fix (only if NODE == dll->tail)

if (node == dll->tail)
    dll->tail = node->prev;

Example (NODE was tail):
Before:
            [SENTINEL]  <->  [N1]  <->  [NODE]
                                          ^
                                          |
                                         tail
After:
            [SENTINEL]  <->  [N1]
                               ^
                               |
                             tail

-------------------------------------------------------------------------------
Step 4: Free memory
free(node);

Final State:
            [SENTINEL]  <->  [N1]  <->  [N3]
                ^                         |
                |_________________________|

-------------------------------------------------------------------------------
Special Case: Only one real node
Before:
            [SENTINEL]  <->  [NODE]
                               ^
                               |
                             tail

After deletion:
            [SENTINEL]
              next ──┐
              prev ──┘

head == tail == SENTINEL

-------------------------------------------------------------------------------
WARNING:
Do NOT call del(dll, dll->head)
Deleting the sentinel corrupts the list.
-------------------------------------------------------------------------------
*/


/* Delete a node from the list. */
void del(dll_t *dll, dll_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (node == dll->tail) { // need to update tail node if we're deleting it
        dll->tail = node->prev;
    }
    free(node);
}


/*
/* Delete all nodes from the list. */
/*
-------------------------------------------------------------------------------
Initial State (example: 3 real nodes)

                dll_t
                                    +------------------+
                                    | tail ─────────┐  |
                                    |               |  |
                                    +---------------|--+
                                                    |
                                                    v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->  [N1]  <->  [N2]  <->  [N3]
                ^                                    |
                |____________________________________|

Code:
node = dll->head->next;        // first real node (N1)
while (node != dll->head) {    // stop when we get back to SENTINEL
    next = node->next;         // save next BEFORE deleting node
    del(dll, node);            // unlink + free current
    node = next;               // advance
}

-------------------------------------------------------------------------------
Step 0: Start iteration
node = dll->head->next;

                dll_t
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->  [N1]  <->  [N2]  <->  [N3]
                              ^
                              |
                             node

-------------------------------------------------------------------------------
Iteration 1
next = node->next;     (next = N2)
del(dll, node);        (delete N1)
node = next;           (node = N2)

Resulting ring:

                dll_t
                            +------------------+
                            | tail ─────────┐  |
                            |               |  |
                            +---------------|--+
                                            |
                                            v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->  [N2]  <->  [N3]
                              ^
                              |
                             node
                ^                         |
                |_________________________|

-------------------------------------------------------------------------------
Iteration 2
next = node->next;     (next = N3)
del(dll, node);        (delete N2)
node = next;           (node = N3)

Resulting ring:

                dll_t
                +------------------+
                | tail ─────────┐  |
                |               |  |
                +---------------|--+
                                |
                                v
+------------------+
| head ─────────┐  |
|               |  |
+---------------|--+
                |
                v
            [SENTINEL]  <->  [N3]
                              ^
                              |
                             node
                ^              |
                |______________|

-------------------------------------------------------------------------------
Iteration 3
next = node->next;     (next = SENTINEL)
del(dll, node);        (delete N3)  <-- this also moves tail back to SENTINEL
node = next;           (node = SENTINEL)

Resulting ring (empty list again):

dll_t
+------------------+
| head ─────────┐  |
| tail ─────────┘  |
+------------------+
                  v
            +------------------+
            |   SENTINEL (S)   |
            |------------------|
            | next ─────────┐  |
            | prev ─────────┘  |
            +------------------+
                ^         |
                |_________|

Loop ends because:
node == dll->head   (SENTINEL)

-------------------------------------------------------------------------------
Final Cleanup (IMPORTANT)

Your current code:
    free(dll);

This LEAKS the sentinel node (dll->head).

Correct cleanup should be:
    free(dll->head);   // free sentinel
    free(dll);         // free dll_t

-------------------------------------------------------------------------------
Notes / Safety:
- del() must never be called on the sentinel.
- Saving "next" before del() is mandatory (del frees current node).
-------------------------------------------------------------------------------

*/
/* Delete all nodes from the list. */
void deinit_dll(dll_t *dll) {
    dll_node_t *node = dll->head->next;
    while (node != dll->head) {
        dll_node_t *next = node->next;
        del(dll, node);
        node = next;
    }
    free(dll);
}
