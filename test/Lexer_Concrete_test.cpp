#include "TestHarness.h"
#include "MockSupport.h"
#include "Error_Mock.h"

extern "C"
{
#include <limits.h>
#include <string.h>
#include "Lexer_Concrete.h"
#include "List_Calloc.h"
#include "Token.h"
}

#define LENGTH_1 (1)
#define LENGTH_2 (2)
#define LINE_1 (1)

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

   void ShouldReportThisErrorNTimes(const char *message, int N)
   {
      mock()
         .expectNCalls(N, "report")
         .onObject(&errorMock)
         .withParameter("message", message);
   }

   void TheTokenAtThisIndexShouldBe(size_t index, Token_t *expectedToken)
   {
      Token_t *actualToken;
      List_At(&tokens.interface, index, &actualToken);
      MEMCMP_EQUAL(expectedToken, actualToken, sizeof(Token_t));
   }

   void TheResultingTokensShouldBe(const Token_t *expectedTokens, size_t length)
   {
      for(int i = 0; i < length; i++)
      {
         TheTokenAtThisIndexShouldBe(i, &expectedTokens[i]);
      }
   }
};

/***************************
 * Basic single token test
 ***************************/

TEST(Lexer_Concrete, RecognizesRightCurlyBrace)
{
   const char *source = "}";
   const Token_t expectedToken = { Token_Type_CurlyBrace_Right, source, 1, 1 };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheTokenAtThisIndexShouldBe(0, &expectedToken);
}

/***************************
 * Symbolic tokens tests
 ***************************/
TEST(Lexer_Concrete, RecognizesStringOfNonTouchySymbols)
{
   const char *source = "()[]{},.`";
   const Token_t expectedTokens[] = {
      { Token_Type_Paren_Left,        &source[0], LENGTH_1, LINE_1 },
      { Token_Type_Paren_Right,       &source[1], LENGTH_1, LINE_1 },
      { Token_Type_SquareBrace_Left,  &source[2], LENGTH_1, LINE_1 },
      { Token_Type_SquareBrace_Right, &source[3], LENGTH_1, LINE_1 },
      { Token_Type_CurlyBrace_Left,   &source[4], LENGTH_1, LINE_1 },
      { Token_Type_CurlyBrace_Right,  &source[5], LENGTH_1, LINE_1 },
      { Token_Type_Comma,             &source[6], LENGTH_1, LINE_1 },
      { Token_Type_Dot,               &source[7], LENGTH_1, LINE_1 },
      { Token_Type_Backtick,          &source[8], LENGTH_1, LINE_1 }
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 9);
}

TEST(Lexer_Concrete, RecognizesManyTouchySymbolsWithSpacesInbetween)
{
   const char *source = "@ # $ - + / * = < > <= >= != ==";
   const Token_t expectedTokens[] = {
      { Token_Type_Arroba,             &source[0],  LENGTH_1, LINE_1 },
      { Token_Type_Pound,              &source[2],  LENGTH_1, LINE_1 },
      { Token_Type_Dollar,             &source[4],  LENGTH_1, LINE_1 },
      { Token_Type_Dash,               &source[6],  LENGTH_1, LINE_1 },
      { Token_Type_Plus,               &source[8],  LENGTH_1, LINE_1 },
      { Token_Type_Slash,              &source[10], LENGTH_1, LINE_1 },
      { Token_Type_Asterisk,           &source[12], LENGTH_1, LINE_1 },
      { Token_Type_Equal,              &source[14], LENGTH_1, LINE_1 },
      { Token_Type_AngleBracket_Left,  &source[16], LENGTH_1, LINE_1 },
      { Token_Type_AngleBracket_Right, &source[18], LENGTH_1, LINE_1 },
      { Token_Type_LessEqual,          &source[20], LENGTH_2, LINE_1 },
      { Token_Type_GreaterEqual,       &source[23], LENGTH_2, LINE_1 },
      { Token_Type_BangEqual,          &source[26], LENGTH_2, LINE_1 },
      { Token_Type_EqualEqual,         &source[29], LENGTH_2, LINE_1 }
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 14);
}

/***************************
* Reports Errors
***************************/
TEST(Lexer_Concrete, NoSpaceBetweenTokensRequiringSpaceReportsErrors)
{
   const char *source = "@#$-+ /*=<><=>=!===";
   const Token_t expectedTokens[] = {
      { Token_Type_Arroba,             &source[0],  LENGTH_1, LINE_1 },
      { Token_Type_Pound,              &source[1],  LENGTH_1, LINE_1 },
      { Token_Type_Dollar,             &source[2],  LENGTH_1, LINE_1 },
      { Token_Type_Dash,               &source[3],  LENGTH_1, LINE_1 },
      { Token_Type_Plus,               &source[4],  LENGTH_1, LINE_1 },
      { Token_Type_Slash,              &source[6], LENGTH_1, LINE_1 },
      { Token_Type_Asterisk,           &source[7], LENGTH_1, LINE_1 },
      { Token_Type_Equal,              &source[8], LENGTH_1, LINE_1 },
      { Token_Type_AngleBracket_Left,  &source[9], LENGTH_1, LINE_1 },
      { Token_Type_AngleBracket_Right, &source[10], LENGTH_1, LINE_1 },
      { Token_Type_LessEqual,          &source[11], LENGTH_2, LINE_1 },
      { Token_Type_GreaterEqual,       &source[13], LENGTH_2, LINE_1 },
      { Token_Type_BangEqual,          &source[15], LENGTH_2, LINE_1 },
      { Token_Type_EqualEqual,         &source[17], LENGTH_2, LINE_1 }
   };

   ShouldReportThisError("\"Touchy\" symbol '@' next to another touchy symbol '#'");
   ShouldReportThisError("\"Touchy\" symbol '#' next to other touchy symbols '@' and '$'");
   ShouldReportThisError("\"Touchy\" symbol '$' next to other touchy symbols '#' and '-'");
   ShouldReportThisError("\"Touchy\" symbol '-' next to other touchy symbols '$' and '+'");
   ShouldReportThisError("\"Touchy\" symbol '+' next to another touchy symbol '-'");
   ShouldReportThisError("\"Touchy\" symbol '/' next to another touchy symbol '*'");
   ShouldReportThisError("\"Touchy\" symbol '*' next to other touchy symbols '/' and '='");
   ShouldReportThisError("\"Touchy\" symbol '=' next to other touchy symbols '*' and '<'");
   ShouldReportThisError("\"Touchy\" symbol '<' next to other touchy symbols '=' and '>'");
   ShouldReportThisError("\"Touchy\" symbol '>' next to other touchy symbols '<' and '<'");
   ShouldReportThisError("\"Touchy\" symbol '<=' next to other touchy symbols '>' and '>'");
   ShouldReportThisError("\"Touchy\" symbol '>=' next to other touchy symbols '=' and '!'");
   ShouldReportThisError("\"Touchy\" symbol '!=' next to other touchy symbols '=' and '='");
   ShouldReportThisError("\"Touchy\" symbol '==' next to another touchy symbol '='");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 14);
}

TEST(Lexer_Concrete, ReportsErrorForUnexpectedCharacters)
{
   const char *source = "%^&\\|;";

   ShouldReportThisError("Unexpected character '%'");
   ShouldReportThisError("Unexpected character '^'");
   ShouldReportThisError("Unexpected character '&'");
   ShouldReportThisError("Unexpected character '\\'");
   ShouldReportThisError("Unexpected character '|'");
   ShouldReportThisError("Unexpected character ';'");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
}

TEST(Lexer_Concrete, ReportsErrorForUnexpectedNonPrintableCharacter)
{
   char nonprintables[] = {
      0x01, // start of heading
      0x02, // start of text
      0x03, // end of text
      0x04, // end of transmission
      0x05, // enquiry
      0x06, // ACK
      0x07, // bell
      0x08, // backspace
      0x0E, // shift out
      0x0F, // shift in
      0x10, // data link escape
      0x11, // device control 1
      0x12, // device control 2
      0x13, // device control 3
      0x14, // device control 4
      0x15, // NAK
      0x16, // SYN
      0x17, // end of transmission
      0x18, // cancel
      0x19, // end of medium
      0x1A, // substitute
      0x1B, // escape
      0x1C, // file separator
      0x1D, // group separator
      0x1E, // record separator
      0x1F, // unit separator
      0x7F, // DEL
      0x00  // null
   };

   ShouldReportThisErrorNTimes("Unexpected non-printable character", strlen(nonprintables));
   Lexer_Lex(&lexer.interface, nonprintables, &tokens.interface);
}

IGNORE_TEST(Lexer_Concrete, ReportsErrorForNonAscii)
{

}

// Special Case tests
IGNORE_TEST(Lexer_Concrete, SpecialCase_Dot)
{

}
