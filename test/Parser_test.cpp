#include "TestHarness.h"

extern "C"
{
   #include "Parser.h"
}

TEST_GROUP(Parser)
{
   void setup()
   {
   }
};

IGNORE_TEST(Parser, FailImmediately)
{
   FAIL("Immediately");
}
