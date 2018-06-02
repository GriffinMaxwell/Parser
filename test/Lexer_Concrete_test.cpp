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

   void ShouldThrowThisError(const char *message)
   {
      mock()
         .expectOneCall("report")
         .onObject(&errorMock)
         .withParameter("message", message);
   }

   void TheTokenTypeAtThisIndexShouldBe(size_t index, const Token_Type_t expectedTokenType)
   {
      Token_t *actualToken;
      List_At(&tokens.interface, index, &actualToken);

      CHECK_EQUAL(expectedTokenType, actualToken->type);
   }


   void TheTokenTypeAndLexemeAtThisIndexShouldBe(size_t index, const Token_t *expectedToken)
   {
      Token_t *actualToken;
      List_At(&tokens.interface, index, &actualToken);

      CHECK_EQUAL(expectedToken->type, actualToken->type);
      STRCMP_EQUAL(expectedToken->lexeme, actualToken->lexeme);
   }

   void TheTokenTypeAndValueAtThisIndexShouldBe(size_t index, const Token_t *expectedToken)
   {
      Token_t *actualToken;
      List_At(&tokens.interface, index, &actualToken);

      CHECK_EQUAL(expectedToken->type, actualToken->type);
      CHECK_EQUAL(expectedToken->value, actualToken->value);
   }

   void TheTokenTypesForThisListOfTokensShouldBe(const Token_Type_t *tokens, size_t length)
   {
      for(int i = 0; i < length; i++)
      {
         TheTokenTypeAtThisIndexShouldBe(i, tokens[i]);
      }
   }
};

/***************************
 * Single token tests
 ***************************/

TEST(Lexer_Concrete, RecognizesLeftParen)
{
   const Token_Type_t expectedTokenType = Token_Type_Paren_Left;

   Lexer_Lex(&lexer.interface, "(", &tokens.interface);
   TheTokenTypeAtThisIndexShouldBe(0, expectedTokenType);
}

TEST(Lexer_Concrete, RecognizesRightParen)
{
   const Token_Type_t expectedTokenType = Token_Type_Paren_Right;

   Lexer_Lex(&lexer.interface, ")", &tokens.interface);
   TheTokenTypeAtThisIndexShouldBe(0, expectedTokenType);
}

TEST(Lexer_Concrete, RecognizesLeftSquareBrace)
{
   const Token_Type_t expectedTokenType = Token_Type_SquareBrace_Left;

   Lexer_Lex(&lexer.interface, "[", &tokens.interface);
   TheTokenTypeAtThisIndexShouldBe(0, expectedTokenType);
}

TEST(Lexer_Concrete, RecognizesRightSquareBrace)
{
   const Token_Type_t expectedTokenType = Token_Type_SquareBrace_Right;

   Lexer_Lex(&lexer.interface, "]", &tokens.interface);
   TheTokenTypeAtThisIndexShouldBe(0, expectedTokenType);
}

TEST(Lexer_Concrete, RecognizesLeftCurlyBrace)
{
   const Token_Type_t expectedTokenType = Token_Type_CurlyBrace_Left;

   Lexer_Lex(&lexer.interface, "{", &tokens.interface);
   TheTokenTypeAtThisIndexShouldBe(0, expectedTokenType);
}

TEST(Lexer_Concrete, RecognizesRightCurlyBrace)
{
   const Token_Type_t expectedTokenType = Token_Type_CurlyBrace_Right;

   Lexer_Lex(&lexer.interface, "}", &tokens.interface);
   TheTokenTypeAtThisIndexShouldBe(0, expectedTokenType);
}

/***************************
 * String of single token tests
 ***************************/

TEST(Lexer_Concrete, RecognizesStringOfSimpleTokens)
{
   const Token_Type_t expectedTokens[] = {
      Token_Type_CurlyBrace_Right,
      Token_Type_CurlyBrace_Left,
      Token_Type_SquareBrace_Left,
      Token_Type_Arroba,
      Token_Type_Comma,
      Token_Type_Backtick,
   };

   Lexer_Lex(&lexer.interface, "}{[@,`", &tokens.interface);
   TheTokenTypesForThisListOfTokensShouldBe(expectedTokens, 6);
}

// TEST(Lexer_Concrete, FailImmediately)
// {
//    FAIL("Immediately");
// }
