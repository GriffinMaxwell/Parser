/***
 * File: Lexer_Concrete.c
 */

#include <stdbool.h>
#include <ctype.h>
#include "Lexer_Concrete.h"
#include "Token.h"
#include "util.h"

static struct
{
   bool isValid;
   Token_Type_t tokenType;
   bool mustBeSurroundedByWhitespace;
   uint8_t maxNumberOfRepetitions;
} singleSymbolSyntaxRules[128] = { 0 };

static const char *sourceStringCharacterPointer;

static void FillSingleSymbolSyntaxRuleArray()
{
   singleSymbolSyntaxRules['('].isValid = true;
   singleSymbolSyntaxRules['('].tokenType = Token_Type_Paren_Left;
   singleSymbolSyntaxRules['('].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules['('].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules[')'].isValid = true;
   singleSymbolSyntaxRules[')'].tokenType = Token_Type_Paren_Right;
   singleSymbolSyntaxRules[')'].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules[')'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['['].isValid = true;
   singleSymbolSyntaxRules['['].tokenType = Token_Type_SquareBrace_Left;
   singleSymbolSyntaxRules['['].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules['['].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules[']'].isValid = true;
   singleSymbolSyntaxRules[']'].tokenType = Token_Type_SquareBrace_Right;
   singleSymbolSyntaxRules[']'].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules[']'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['{'].isValid = true;
   singleSymbolSyntaxRules['{'].tokenType = Token_Type_CurlyBrace_Left;
   singleSymbolSyntaxRules['{'].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules['{'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['}'].isValid = true;
   singleSymbolSyntaxRules['}'].tokenType = Token_Type_CurlyBrace_Right;
   singleSymbolSyntaxRules['}'].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules['}'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules[','].isValid = true;
   singleSymbolSyntaxRules[','].tokenType = Token_Type_Comma;
   singleSymbolSyntaxRules[','].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules[','].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['.'].isValid = true;
   singleSymbolSyntaxRules['.'].tokenType = Token_Type_Dot;
   singleSymbolSyntaxRules['.'].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules['.'].maxNumberOfRepetitions = 3;

   singleSymbolSyntaxRules['?'].isValid = true;
   singleSymbolSyntaxRules['?'].tokenType = Token_Type_Question;
   singleSymbolSyntaxRules['?'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['?'].maxNumberOfRepetitions = 255;  // Should technically be unlimited

   singleSymbolSyntaxRules['`'].isValid = true;
   singleSymbolSyntaxRules['`'].tokenType = Token_Type_Backtick;
   singleSymbolSyntaxRules['`'].mustBeSurroundedByWhitespace = false;
   singleSymbolSyntaxRules['`'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['@'].isValid = true;
   singleSymbolSyntaxRules['@'].tokenType = Token_Type_Arroba;
   singleSymbolSyntaxRules['@'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['@'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['#'].isValid = true;
   singleSymbolSyntaxRules['#'].tokenType = Token_Type_Pound;
   singleSymbolSyntaxRules['#'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['#'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['$'].isValid = true;
   singleSymbolSyntaxRules['$'].tokenType = Token_Type_Dollar;
   singleSymbolSyntaxRules['$'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['$'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules[':'].isValid = true;
   singleSymbolSyntaxRules[':'].tokenType = Token_Type_Colon;
   singleSymbolSyntaxRules[':'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules[':'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['-'].isValid = true;
   singleSymbolSyntaxRules['-'].tokenType = Token_Type_Dash;
   singleSymbolSyntaxRules['-'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['-'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['+'].isValid = true;
   singleSymbolSyntaxRules['+'].tokenType = Token_Type_Plus;
   singleSymbolSyntaxRules['+'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['+'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['/'].isValid = true;
   singleSymbolSyntaxRules['/'].tokenType = Token_Type_Slash;
   singleSymbolSyntaxRules['/'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['/'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['*'].isValid = true;
   singleSymbolSyntaxRules['*'].tokenType = Token_Type_Asterisk;
   singleSymbolSyntaxRules['*'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['*'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['='].isValid = true;
   singleSymbolSyntaxRules['='].tokenType = Token_Type_Equal;
   singleSymbolSyntaxRules['='].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['='].maxNumberOfRepetitions = 2;

   singleSymbolSyntaxRules['<'].isValid = true;
   singleSymbolSyntaxRules['<'].tokenType = Token_Type_AngleBracket_Left;
   singleSymbolSyntaxRules['<'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['<'].maxNumberOfRepetitions = 1;

   singleSymbolSyntaxRules['>'].isValid = true;
   singleSymbolSyntaxRules['>'].tokenType = Token_Type_AngleBracket_Right;
   singleSymbolSyntaxRules['>'].mustBeSurroundedByWhitespace = true;
   singleSymbolSyntaxRules['>'].maxNumberOfRepetitions = 1;
}

static inline void AdvanceOne()
{
   sourceStringCharacterPointer++;
}

static inline char PeekPrevious(const char *source)
{
   if(sourceStringCharacterPointer == source)
   {
      return '\0';   // Return null terminator if at the beginning of the string
   }
   else
   {
      return *(sourceStringCharacterPointer - 1);
   }
}

static inline char Peek()
{
   return *sourceStringCharacterPointer;
}

static inline char PeekNext()
{
   return sourceStringCharacterPointer[1];
}

static bool isIdentifier(char current)
{
   // Kludgy rules for non-alpha characters that can appear in an identifier,
   // including the question mark, which can appear more than once in a row
   return (isalpha(current)
      || ((current == '_'
            || current == '-'
            || current == '#'
            || current == '!'
            || current == '?')
         && !(isspace(PeekNext())
            || PeekNext() == '?')));
}

static bool isMultiCharacterSymbol(char current)
{
   return ((current == '<'
         || current == '>'
         || current == '!')
      && PeekNext() == '=');
}

static bool isSingleCharacterSymbol(char current)
{
   return singleSymbolSyntaxRules[current].isValid;
}

static void ConsumeSingleCharacterSymbol(char current, I_List_t *tokenList)
{
   Token_t singleCharacterSymbol;
   singleCharacterSymbol.type = singleSymbolSyntaxRules[current].tokenType;
   singleCharacterSymbol.value = 0;

   List_Add(tokenList, &singleCharacterSymbol);
   AdvanceOne();
}

static void HandleUnknownSymbolError(char current, I_Error_t * errorHandler, I_List_t *tokenList)
{
   char message[26] = "Error: Invalid symbol ' '";
   message[24] = current;
   Error_Report(errorHandler, message);

   Token_t unknown = { .type = Token_Type_Unknown, .value = 0 };
   List_Add(tokenList, &unknown);
}

static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokenList)
{
   // Check identifier or literal (peek ahead and consume if necessary)
   // Check multi-character symbols (peek ahead if necessary)
   // Check single character symbols
   // Else unknown
   REINTERPRET(instance, interface, Lexer_Concrete_t *);
   sourceStringCharacterPointer = source;

   FillSingleSymbolSyntaxRuleArray(); // Make a big static table indexed by the characters themselves

   char current;
   while((current = Peek()) != '\0')
   {
      if(isspace(current))
      {
         // Do nothing.
      }
      if(isIdentifier(current))
      {
         // ConsumeIdentifier();
      }
      else if(isMultiCharacterSymbol(current))
      {
         // ConsumeMultiCharacterSymbol();
      }
      else if(isSingleCharacterSymbol(current))
      {
         ConsumeSingleCharacterSymbol(current, tokenList);
      }
      else
      {
         HandleUnknownSymbolError(current, instance->errorHandler, tokenList);
      }
   }
}

void Lexer_Concrete_Init(Lexer_Concrete_t *instance, I_Error_t *errorHandler)
{
   instance->interface.lex = &lex;

   instance->errorHandler = errorHandler;
}
