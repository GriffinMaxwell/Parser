/***
 * File: I_List.h
 * Desc: Interface functions for accessing and setting items in a dynamic list.
 */
#ifndef _I_LIST_H
#define _I_LIST_H

#include <stddef.h>

typedef struct I_List_t
{
   /*
    * Point to the item at the specified index.
    *
    * @post If index is out of range, item is NULL.
    */
   void (*at)(struct I_List_t *interface, size_t index, void **item);

   /*
    * Sets the item at the specified index.
    *
    * @post Resizes list to guarantee index is valid location.
    */
   void (*set)(struct I_List_t *interface, size_t index, void *item);

   /*
    * Adds an item to the end of a list, growing its size by one.
    */
   void (*add)(struct I_List_t *interface, void *item);
} I_List_t;

#define List_At(interface, index, item) \
   (interface)->at((interface), (index), (item))

#define List_Set(interface, index, item) \
   (interface)->set((interface), (index), (item))

#define List_Add(interface, item) \
   (interface)->add((interface), (item))

#endif
