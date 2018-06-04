#include "TestHarness.h"
#include "MockSupport.h"
#include "Error_Mock.h"

extern "C"
{
#include "Lexer_Concrete.h"
#include "List_Calloc.h"
#include "Token.h"
}

TEST_GROUP(Lexer_Concrete)
{
   Error_Mock_t errorMock;
   List_Calloc_t tokens;

   Lexer_Concrete_t lexer;

   void setup()
   {
      Error_Mock_Init(&errorMock);
      List_Calloc_Init(&tokens, sizeof(Token_t));

      Lexer_Concrete_Init(&lexer, &errorMock.interface);
   }

   void teardown()
   {
      List_Calloc_Deinit(&tokens);
      mock().clear();
   }

   void ShouldReportThisError(const char *message)
   {
      mock()
         .expectOneCall("report")
         .onObject(&errorMock)
         .withParameter("message", message);
   }

   void TheTokenAtThisIndexShouldBe(size_t index, Token_t *expectedToken)
   {
      Token_t *actualToken;
      List_At(&tokens.interface, index, &actualToken);
      MEMCMP_EQUAL(expectedToken, actualToken, sizeof(Token_t));
   }

   void TheLexedListOfTokensShouldBe(const Token_t *expectedTokens, size_t length)
   {
      for(int i = 0; i < length; i++)
      {
         TheTokenAtThisIndexShouldBe(i, &expectedTokens[i]);
      }
   }
};

/***************************
 * Single token tests
 ***************************/

IGNORE_TEST(Lexer_Concrete, RecognizesRightCurlyBrace)
{
   const Token_t expectedToken = { Token_Type_CurlyBrace_Right, 0 };

   Lexer_Lex(&lexer.interface, "}", &tokens.interface);
   TheTokenAtThisIndexShouldBe(0, &expectedToken);
}

/***************************
 * Symbolic tokens tests
 ***************************/

TEST(Lexer_Concrete, RecognizesStringOfNonSpacedTokens)
{
   const char *source = "()[]{},.`";
   const Token_t expectedTokens[] = {
      { Token_Type_Paren_Left, 0 },
      { Token_Type_Paren_Right, 0 },
      { Token_Type_SquareBrace_Left, 0 },
      { Token_Type_SquareBrace_Right, 0 },
      { Token_Type_CurlyBrace_Left, 0 },
      { Token_Type_CurlyBrace_Right, 0 },
      { Token_Type_Comma, 0 },
      { Token_Type_Dot, 0 },
      { Token_Type_Backtick, 0 }
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheLexedListOfTokensShouldBe(expectedTokens, 9);
}

IGNORE_TEST(Lexer_Concrete, NoSpacesBetweenTokensRequiringWhitespaceReportsErrors)
{
   const char *source = "?@#$:-+/*=<><=>=!===";

   ShouldReportThisError("Error");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
}
