#include "TestHarness.h"

extern "C"
{
   #include <stdint.h>
   #include <string.h>
   #include "List_Calloc.h"
}

#define ITEM_SIZE (10)

typedef struct
{
   uint8_t byte[ITEM_SIZE];
} Item_t;

TEST_GROUP(List_Calloc)
{
   List_Calloc_t list;
   Item_t *readItem;
   Item_t dummyItem;

   void setup()
   {
      List_Calloc_Init(&list, sizeof(Item_t));
      readItem = NULL;
      memset((void *)&dummyItem, 0xAC, ITEM_SIZE);
   }

   void teardown()
   {
      List_Calloc_Deinit(&list);
   }

   void AfterSettingTheReadItemToADummyAddress()
   {
      readItem = &dummyItem;
   }

   void TheReadItemShouldPointTo(Item_t *expectedItem)
   {
      CHECK_EQUAL(expectedItem, readItem);
   }

   void TheReadItemShouldEqual(Item_t expectedItem)
   {
      MEMCMP_EQUAL(&expectedItem, readItem, ITEM_SIZE)
   }
};

TEST(List_Calloc, EmptyListReturnsNullAtIndex0)
{
   AfterSettingTheReadItemToADummyAddress();
   List_At(&list.interface, 0, (void **)&readItem);
   TheReadItemShouldPointTo(NULL);
}

TEST(List_Calloc, EmptyListReturnsNullAtIndex100)
{
   AfterSettingTheReadItemToADummyAddress();
   List_At(&list.interface, 100, (void **)&readItem);
   TheReadItemShouldPointTo(NULL);
}

TEST(List_Calloc, ListWithOneAddedItemReturnsThatItemAtIndex0)
{
   List_Add(&list.interface, (void *)&dummyItem);
   List_At(&list.interface, 0, (void **)&readItem);

   TheReadItemShouldEqual(dummyItem);
}

TEST(List_Calloc, ListWithTwoAddedItemsReturnsThatItemAtIndex0and1)
{
   List_Add(&list.interface, (void *)&dummyItem);
   List_Add(&list.interface, (void *)&dummyItem);

   List_At(&list.interface, 0, (void **)&readItem);
   TheReadItemShouldEqual(dummyItem);

   List_At(&list.interface, 1, (void **)&readItem);
   TheReadItemShouldEqual(dummyItem);
}

TEST(List_Calloc, SetAtHighIndexResizesTheListTo)
{
   List_Set(&list.interface, 20, (void *)&dummyItem);
   List_At(&list.interface, 20, (void **)&readItem);
   TheReadItemShouldEqual(dummyItem);

   List_At(&list.interface, 21, (void **)&readItem);
   TheReadItemShouldPointTo(NULL);
}
