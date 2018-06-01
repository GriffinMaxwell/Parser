/***
 * File: List_Calloc.h
 * Desc: Implements list interface using std::calloc for storage.
 */

#ifndef _LIST_CALLOC_H
#define _LIST_CALLOC_H

#include <stdint.h>
#include "I_List.h"

typedef struct
{
   I_List_t interface;

   size_t itemSize;
   size_t usedSize;
   size_t allocatedSize;

   uint8_t *storage;
} List_Calloc_t;

/*
 * Initialize a List_Calloc
 *
 * @param itemSize - size of each list item in bytes
 */
void List_Calloc_Init(List_Calloc_t *instance, size_t itemSize);

/*
 * Deinitialize a List_Calloc
 */
void List_Calloc_Deinit(List_Calloc_t *instance);

#endif
