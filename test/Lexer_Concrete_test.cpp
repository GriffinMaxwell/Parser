#include "TestHarness.h"
#include "MockSupport.h"
#include "Error_Mock.h"

extern "C"
{
#include <string.h>
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

   void ShouldReportThisError(size_t line, const char *message)
   {
      mock()
         .expectOneCall("report")
         .onObject(&errorMock)
         .withParameter("line", line)
         .withParameter("message", message);
   }

   void ShouldReportThisErrorNTimes(size_t line, const char *message, int N)
   {
      mock()
         .expectNCalls(N, "report")
         .onObject(&errorMock)
         .withParameter("line", line)
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
 * Symbol tokens tests
 ***************************/
TEST(Lexer_Concrete, RecognizesStringOfNonTouchySymbols)
{
   const char *source = "()[]{},.`";
   const Token_t expectedTokens[] = {
      { Token_Type_Paren_Left,        &source[0], 1, 1 },
      { Token_Type_Paren_Right,       &source[1], 1, 1 },
      { Token_Type_SquareBrace_Left,  &source[2], 1, 1 },
      { Token_Type_SquareBrace_Right, &source[3], 1, 1 },
      { Token_Type_CurlyBrace_Left,   &source[4], 1, 1 },
      { Token_Type_CurlyBrace_Right,  &source[5], 1, 1 },
      { Token_Type_Comma,             &source[6], 1, 1 },
      { Token_Type_Dot,               &source[7], 1, 1 },
      { Token_Type_Backtick,          &source[8], 1, 1 }
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 9);
}

TEST(Lexer_Concrete, RecognizesTouchySymbolsWithSpacesInbetween)
{
   const char *source = "@ # $ - + / * = < > <= >= != ==";
   const Token_t expectedTokens[] = {
      { Token_Type_Arroba,             &source[0],  1, 1 },
      { Token_Type_Pound,              &source[2],  1, 1 },
      { Token_Type_Dollar,             &source[4],  1, 1 },
      { Token_Type_Dash,               &source[6],  1, 1 },
      { Token_Type_Plus,               &source[8],  1, 1 },
      { Token_Type_Slash,              &source[10], 1, 1 },
      { Token_Type_Asterisk,           &source[12], 1, 1 },
      { Token_Type_Equal,              &source[14], 1, 1 },
      { Token_Type_AngleBracket_Left,  &source[16], 1, 1 },
      { Token_Type_AngleBracket_Right, &source[18], 1, 1 },
      { Token_Type_LessEqual,          &source[20], 2, 1 },
      { Token_Type_GreaterEqual,       &source[23], 2, 1 },
      { Token_Type_BangEqual,          &source[26], 2, 1 },
      { Token_Type_EqualEqual,         &source[29], 2, 1 }
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 14);
}

/***************************
* Identifiers
***************************/
TEST(Lexer_Concrete, RecongnizesAllKindsOfStandardIdentifiers)
{
   const char *source = "word _attribute ???????? #-items valid-identifier? t######## ####t";
   const Token_t expectedTokens[] = {
      { Token_Type_Identifier, &source[0],  4,  1 },
      { Token_Type_Identifier, &source[5],  10, 1 },
      { Token_Type_Identifier, &source[16], 8,  1 },
      { Token_Type_Identifier, &source[25], 6,  1 },
      { Token_Type_Identifier, &source[33], 17, 1 },
      { Token_Type_Identifier, &source[51], 9,  1 },
      { Token_Type_Identifier, &source[61], 5,  1 }
   };
}

/***************************
* Literals
***************************/
TEST(Lexer_Concrete, RecognizesStringLiteral)
{
   const char *source = "  \"This is a string literal\"";
   const Token_t expectedToken = { Token_Type_Literal_String, &source[2], 26, 1};

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheTokenAtThisIndexShouldBe(0, &expectedToken);
}

TEST(Lexer_Concrete, RecognizesSymbolicLiteral)
{
   const char *source = ":this-is-a-symbolic-literal?";
   const Token_t expectedToken = { Token_Type_Literal_Symbol, &source[0], 28, 1};

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheTokenAtThisIndexShouldBe(0, &expectedToken);
}

TEST(Lexer_Concrete, RecognizesNumberLiterals)
{
   const char *source = "1 2.3 .4 567. 9384573940.2832985723.5432";
   const Token_t expectedTokens[] = {
      { Token_Type_Literal_Number, &source[0],  1,  1 },
      { Token_Type_Literal_Number, &source[2],  3,  1 },
      { Token_Type_Literal_Number, &source[6],  2,  1 },
      { Token_Type_Literal_Number, &source[9],  4,  1 },
      { Token_Type_Literal_Number, &source[14], 21, 1 },
      { Token_Type_Literal_Number, &source[35], 5,  1 }
   };

   ShouldReportThisError(1, "Missing space before decimal number with no leading zero");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 6);
}

/***************************
* Special case characters
***************************/
TEST(Lexer_Concrete, SpecialCase_Dot)
{
   const char *source = ". .. ... .6 ..6 ...";
   const Token_t expectedTokens[] = {
      { Token_Type_Dot,            &source[0],  1, 1 },
      { Token_Type_DotDot,         &source[2],  2, 1 },
      { Token_Type_DotDotDot,      &source[5],  3, 1 },
      { Token_Type_Literal_Number, &source[9],  2, 1 },
      { Token_Type_DotDot,         &source[12], 2, 1 },
      { Token_Type_Literal_Number, &source[14], 1, 1 },
      { Token_Type_DotDotDot,      &source[16], 3, 1 }
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 6);
}

TEST(Lexer_Concrete, SpecialCase_Colon)
{
   const char *source = ": :this-is-a-symbolic-literal?:";
   const Token_t expectedTokens[] = {
      { Token_Type_Colon, &source[0], 1, 1},
      { Token_Type_Literal_Symbol, &source[2], 28, 1},
      { Token_Type_Colon, &source[30], 1, 1}
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 3);
}

TEST(Lexer_Concrete, SpecialCase_Dash)
{
   const char *source = "-one-name- - another-name";
   const Token_t expectedTokens[] = {
      { Token_Type_Identifier, &source[0],  10, 1},
      { Token_Type_Dash,       &source[11], 1,  1},
      { Token_Type_Identifier, &source[13], 12, 1}
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 3);
}

TEST(Lexer_Concrete, SpecialCase_Exclamation)
{
   const char *source = "!one!name! != another!name";
   const Token_t expectedTokens[] = {
      { Token_Type_Identifier, &source[0],  10, 1},
      { Token_Type_BangEqual,  &source[11], 2,  1},
      { Token_Type_Identifier, &source[14], 12, 1}
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 3);
}

TEST(Lexer_Concrete, SpecialCase_Pound)
{
   const char *source = "#one#name# # another#name";
   const Token_t expectedTokens[] = {
      { Token_Type_Identifier, &source[0],  10, 1},
      { Token_Type_Pound,      &source[11], 1,  1},
      { Token_Type_Identifier, &source[13], 12, 1}
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 3);
}

TEST(Lexer_Concrete, SpecialCase_Tilde)
{
   const char *source = "~one~name~ ~ another~name";
   const Token_t expectedTokens[] = {
      { Token_Type_Identifier, &source[0],  1, 1},
      { Token_Type_Identifier, &source[1],  3, 1},
      { Token_Type_Identifier, &source[4],  1, 1},
      { Token_Type_Identifier, &source[5],  4, 1},
      { Token_Type_Identifier, &source[9],  1, 1},
      { Token_Type_Identifier, &source[11], 1, 1},
      { Token_Type_Identifier, &source[13], 7, 1},
      { Token_Type_Identifier, &source[20], 1, 1},
      { Token_Type_Identifier, &source[21], 4, 1}
   };

   ShouldReportThisError(1, "\"Touchy\" symbol '~' next to another touchy symbol 'o'");
   ShouldReportThisError(1, "\"Touchy\" symbol '~' next to other touchy symbols 'e' and 'n'");
   ShouldReportThisError(1, "\"Touchy\" symbol '~' next to another touchy symbol 'e'");
   ShouldReportThisError(1, "\"Touchy\" symbol '~' next to other touchy symbols 'r' and 'n'");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 3);
}

/***************************
* Reports Errors
***************************/
TEST(Lexer_Concrete, BadIdentifierNameReportsErrors)
{
   const char *source = "____ #### --_- !!";

   ShouldReportThisError(1, "Identifier name missing [a-zA-Z?]: '____'");
   ShouldReportThisError(1, "Identifier name missing [a-zA-Z?]: '####'");
   ShouldReportThisError(1, "Identifier name missing [a-zA-Z?]: '--_-'");
   ShouldReportThisError(1, "Identifier name missing [a-zA-Z?]: '!!'");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
}

TEST(Lexer_Concrete, BadSymbolicLiteralNameReportsErrors)
{
   const char *source = ":____ :#### :--_- :!!";

   ShouldReportThisError(1, "Symbol name missing [a-zA-Z?]: ':____'");
   ShouldReportThisError(1, "Symbol name missing [a-zA-Z?]: ':####'");
   ShouldReportThisError(1, "Symbol name missing [a-zA-Z?]: ':--_-'");
   ShouldReportThisError(1, "Symbol name missing [a-zA-Z?]: ':!!'");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
}

TEST(Lexer_Concrete, NoSpaceBetweenTouchySymbolsReportsErrors)
{
   const char *source = "@#$-+ /*=<><=>=!===";
   const Token_t expectedTokens[] = {
      { Token_Type_Arroba,             &source[0],  1, 1 },
      { Token_Type_Pound,              &source[1],  1, 1 },
      { Token_Type_Dollar,             &source[2],  1, 1 },
      { Token_Type_Dash,               &source[3],  1, 1 },
      { Token_Type_Plus,               &source[4],  1, 1 },
      { Token_Type_Slash,              &source[6],  1, 1 },
      { Token_Type_Asterisk,           &source[7],  1, 1 },
      { Token_Type_Equal,              &source[8],  1, 1 },
      { Token_Type_AngleBracket_Left,  &source[9],  1, 1 },
      { Token_Type_AngleBracket_Right, &source[10], 1, 1 },
      { Token_Type_LessEqual,          &source[11], 2, 1 },
      { Token_Type_GreaterEqual,       &source[13], 2, 1 },
      { Token_Type_BangEqual,          &source[15], 2, 1 },
      { Token_Type_EqualEqual,         &source[17], 2, 1 }
   };

   ShouldReportThisError(1, "\"Touchy\" symbol '@' next to another touchy symbol '#'");
   ShouldReportThisError(1, "\"Touchy\" symbol '#' next to other touchy symbols '@' and '$'");
   ShouldReportThisError(1, "\"Touchy\" symbol '$' next to other touchy symbols '#' and '-'");
   ShouldReportThisError(1, "\"Touchy\" symbol '-' next to other touchy symbols '$' and '+'");
   ShouldReportThisError(1, "\"Touchy\" symbol '+' next to another touchy symbol '-'");
   ShouldReportThisError(1, "\"Touchy\" symbol '/' next to another touchy symbol '*'");
   ShouldReportThisError(1, "\"Touchy\" symbol '*' next to other touchy symbols '/' and '='");
   ShouldReportThisError(1, "\"Touchy\" symbol '=' next to other touchy symbols '*' and '<'");
   ShouldReportThisError(1, "\"Touchy\" symbol '<' next to other touchy symbols '=' and '>'");
   ShouldReportThisError(1, "\"Touchy\" symbol '>' next to other touchy symbols '<' and '<'");
   ShouldReportThisError(1, "\"Touchy\" symbol '<=' next to other touchy symbols '>' and '>'");
   ShouldReportThisError(1, "\"Touchy\" symbol '>=' next to other touchy symbols '=' and '!'");
   ShouldReportThisError(1, "\"Touchy\" symbol '!=' next to other touchy symbols '=' and '='");
   ShouldReportThisError(1, "\"Touchy\" symbol '==' next to another touchy symbol '='");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 14);
}

TEST(Lexer_Concrete, NoSpaceBetweenIdentifiersNumbersAndTouchySymbolsReportsErrors)
{
   const char *source = "num= 5\nnum-two=num+10";
   const Token_t expectedTokens[] = {
      { Token_Type_Identifier,     &source[0],  3, 1 },
      { Token_Type_Equal,          &source[3],  1, 1 },
      { Token_Type_Literal_Number, &source[5],  1, 1 },

      { Token_Type_Identifier,     &source[7],  7, 2 },
      { Token_Type_Equal,          &source[14], 1, 2 },
      { Token_Type_Identifier,     &source[15], 3, 2 },
      { Token_Type_Plus,           &source[18], 1, 2 },
      { Token_Type_Literal_Number, &source[19], 2, 2 },
   };

   ShouldReportThisError(1, "\"Touchy\" symbol '=' next to another touchy symbol 'm'");
   ShouldReportThisError(2, "\"Touchy\" symbol '=' next to other touchy symbols 'o' and 'n'");
   ShouldReportThisError(2, "\"Touchy\" symbol '+' next to other touchy symbols 'm' and '1'");
   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 8);
}

TEST(Lexer_Concrete, ReportsErrorForUnexpectedCharacters)
{
   const char *source = "%^&\\|;";

   ShouldReportThisError(1, "Unexpected character '%'");
   ShouldReportThisError(1, "Unexpected character '^'");
   ShouldReportThisError(1, "Unexpected character '&'");
   ShouldReportThisError(1, "Unexpected character '\\'");
   ShouldReportThisError(1, "Unexpected character '|'");
   ShouldReportThisError(1, "Unexpected character ';'");
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

   ShouldReportThisErrorNTimes(1, "Unexpected non-printable character", strlen(nonprintables));
   Lexer_Lex(&lexer.interface, nonprintables, &tokens.interface);
}

TEST(Lexer_Concrete, ReportsErrorForNonAscii)
{
   unsigned char nonascii[129];
   int i = 0;
   for(unsigned char c = SCHAR_MAX + 1; c < UCHAR_MAX; c++)
   {
      nonascii[i] = c;
      i++;
   }
   nonascii[128] = '\0';

   ShouldReportThisErrorNTimes(1, "Unexpected non-ascii character", (UCHAR_MAX - SCHAR_MAX));
   Lexer_Lex(&lexer.interface, nonascii, &tokens.interface);
}

/***************************
 * Counts Line Numbers
 ***************************/
TEST(Lexer_Concrete, CountsLineNumbers)
{
   const char *source = "x: int = 5 \ny: int = 10\n print x + y";
   const Token_t expectedTokens[] = {
      { Token_Type_Identifier,     &source[0], 1, 1},
      { Token_Type_Colon,          &source[1], 1, 1},
      { Token_Type_Identifier,     &source[3], 3, 1},
      { Token_Type_Equal,          &source[7], 1, 1},
      { Token_Type_Literal_Number, &source[9], 1, 1},

      { Token_Type_Identifier,     &source[12], 1, 2},
      { Token_Type_Colon,          &source[13], 1, 2},
      { Token_Type_Identifier,     &source[15], 3, 2},
      { Token_Type_Equal,          &source[19], 1, 2},
      { Token_Type_Literal_Number, &source[21], 2, 2},

      { Token_Type_Identifier,     &source[25], 5, 3},
      { Token_Type_Identifier,     &source[31], 1, 3},
      { Token_Type_Plus,           &source[33], 1, 3},
      { Token_Type_Identifier,     &source[35], 1, 3}
   };

   Lexer_Lex(&lexer.interface, source, &tokens.interface);
   TheResultingTokensShouldBe(expectedTokens, 14);
}
