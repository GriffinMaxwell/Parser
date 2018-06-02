#include "TestHarness.h"
#include "Error_TestDouble.h"

extern "C"
{
   #include "Lexer_Concrete.h"
}

TEST_GROUP(Lexer_Concrete)
{
   Error_TestDouble_t errorDouble;

   Lexer_Concrete_t lexer;

   void setup()
   {
      Error_TestDouble_Init(&errorDouble);

      Lexer_Concrete_Init(&lexer, &errorDouble.interface);
   }
};

TEST(Lexer_Concrete, FailImmediately)
{
   FAIL("Immediately");
}
