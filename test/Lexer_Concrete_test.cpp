#include "TestHarness.h"

extern "C"
{
   #include "Lexer_Concrete.h"
}

TEST_GROUP(Lexer_Concrete)
{
   Lexer_Concrete_t lexer;

   void setup()
   {
      Lexer_Concrete_Init(&lexer);
   }
};

TEST(Lexer_Concrete, FailImmediately)
{
   FAIL("Immediately");
}
