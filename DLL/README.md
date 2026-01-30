# Doubly Linked List (DLL)
This folder contains a generic circular doubly linked list implementation used by the IPC demo.

## Structure (Text)
```
dll_t
  head (sentinel node)
    - head->next : first element or head
    - head->prev : last element or head
  tail
    - tail points to last element (or head when empty)

dll_node_t
  data : void*
  next : dll_node_t*
  prev : dll_node_t*
```
The list is circular with a sentinel `head` node. When empty, `head->next == head` and `head->prev == head`, and `tail == head`.

## Operations (Text Diagram)
```
Append (append):
head <-> A <-> B <-> head    +  C
               tail              |
                    after append:
head <-> A <-> B <-> C <-> head
                         tail

Delete (del):
head <-> A <-> B <-> C <-> head
          ^ delete B
head <-> A <-> C <-> head
               tail (if B was not tail)
```

## Testing Method (DLL/main.c)
The `main.c` file is a manual test harness:
- Initializes a DLL and appends integers 0..4.
- Prints the list in reverse order, then verifies each element can be found.
- Deletes nodes 3, 0, and 4, then prints forward and reverse again.
- Calls `deinit_dll` and attempts to print afterward (to surface misuse or crashes).

## Build and Run the DLL Test
From repo root:
```
make dll_test
./build/dll_test
```
