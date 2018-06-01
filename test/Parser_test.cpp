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

TEST(Parser, FailImmediately)
{
   FAIL("Immediately");
}
