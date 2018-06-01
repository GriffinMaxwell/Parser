/***
 * File: I_List.h
 * Desc: Interface functions for getting and setting items in a dynamic list.
 */
#ifndef _I_LIST_H
#define _I_LIST_H

#include <stdint.h>

typedef struct
{
   /*
    * Gets the item at the specified index.
    *
    * @post If index is out of range, item is NULL.
    */
   void (*at)(I_List_t *instance, uint16_t index, void *item);

   /*
    * Sets the item at the specified index.
    *
    * @post Resizes list to guarantee index is valid location.
    */
   void (*set)(I_List_t *instance, uint16_t index, void *item);
} I_List_t;

#define List_At(instance, index, item) \
   instance->at((instance), (index), (item))

#define List_Set(instance, index, item) \
   instance->set((instance), (index), (item))

#endif
