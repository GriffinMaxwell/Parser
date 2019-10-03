/***
 * File: List_Calloc.c
 */
#include <stdlib.h>
#include <string.h> 
#include "List_Calloc.h"
#include "util.h"

static void GrowList(List_Calloc_t *instance, size_t minNewSize)
{
   while(instance->allocatedSize < minNewSize)
   {
      instance->allocatedSize = (instance->allocatedSize + 1) * 3 / 2;
   }

   instance->storage = realloc(instance->storage, (instance->allocatedSize * instance->itemSize));
}

static void set(I_List_t *interface, size_t index, void *item)
{
   REINTERPRET(instance, interface, List_Calloc_t *);

   if(index >= instance->usedSize)
   {
      if(index >= instance->allocatedSize)
      {
         GrowList(instance, index + 1);
      }

      instance->usedSize = index + 1;
   }

   memcpy(&instance->storage[index * instance->itemSize], item, instance->itemSize);
}

static void add(I_List_t *interface, void *item)
{
   REINTERPRET(instance, interface, List_Calloc_t *);

   if(instance->usedSize == instance->allocatedSize)
   {
      GrowList(instance, instance->allocatedSize + 1);
   }

   memcpy(&instance->storage[instance->usedSize * instance->itemSize], item, instance->itemSize);
   instance->usedSize++;
}

static void at(I_List_t *interface, size_t index, void **item)
{
   REINTERPRET(instance, interface, List_Calloc_t *);

   *item = (index < instance->usedSize) ?
      instance->storage + (index * instance->itemSize) :
      NULL;
}

void List_Calloc_Init(List_Calloc_t *instance, size_t itemSize)
{
   instance->interface.at = &at;
   instance->interface.set = &set;
   instance->interface.add = &add;

   instance->itemSize = itemSize;
   instance->usedSize = 0;
   instance->allocatedSize = 0;

   instance->storage = NULL;
}

void List_Calloc_Deinit(List_Calloc_t *instance)
{
   free(instance->storage);
}
