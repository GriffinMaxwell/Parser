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

      mock().strictOrder();
   }

   void teardown()
   {
      List_Calloc_Deinit(&tokens);


      mock().checkExpectations();
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

TEST(Lexer_Concrete, RecognizesStringOfNonSpacedSymbols)
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

TEST(Lexer_Concrete, RecognizesManySymbolsWithSpacesInbetween)
{
   const char *source = "@ # $ - + / * = < > <= >= != == . .. ...";
   const Token_t expectedTokens[] = {
      { Token_Type_Arroba, 0 },
      { Token_Type_Pound, 0 },
      { Token_Type_Dollar, 0 },
      { Token_Type_Dash, 0 },
      { Token_Type_Plus, 0 },
      { Token_Type_Slash, 0 },
      { Token_Type_Asterisk, 0 },
      { Token_Type_Equal, 0 },
      { Token_Type_AngleBracket_Left, 0 },
      { Token_Type_AngleBracket_Right, 0 },
      { Token_Type_LessEqual, 0 },
      { Token_Type_GreaterEqual, 0 },
      { Token_Type_BangEqual, 0 },
      { Token_Type_EqualEqual, 0 },
      { Token_Type_Dot, 0 },
      { Token_Type_DotDot, 0 },
      { Token_Type_DotDotDot, 0 },
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheLexedListOfTokensShouldBe(expectedTokens, 17);
}

TEST(Lexer_Concrete, NoSpaceBetweenTokensRequiringSpaceReportsErrors)
{
   const char *source = "@#$:-+/*=<><=>=!===";

   ShouldReportThisError("");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
}

/***************************
* Unkown symbols tests
***************************/
TEST(Lexer_Concrete, ReportsErrorForEachUnknownSymbol)
{
   const char *source = "%^&\\|;";

   ShouldReportThisError("Unknown symbol '%'");
   ShouldReportThisError("Unknown symbol '^'");
   ShouldReportThisError("Unknown symbol '&'");
   ShouldReportThisError("Unknown symbol '\\'");
   ShouldReportThisError("Unknown symbol '|'");
   ShouldReportThisError("Unknown symbol ';'");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);

}
