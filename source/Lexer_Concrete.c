/***
 * File: Lexer_Concrete.c
 */

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "Lexer_Concrete.h"
#include "Token.h"
#include "util.h"

static struct
{
   bool isValid;
   Token_Type_t tokenType;
   bool canAppearNextToAnyToken;
} singleSymbolSyntaxRules[128] = { 0 };

static const char *sourceStringCharacterPointer;

static void FillSingleSymbolSyntaxRuleArray()
{
   singleSymbolSyntaxRules['('].isValid = true;
   singleSymbolSyntaxRules['('].tokenType = Token_Type_Paren_Left;
   singleSymbolSyntaxRules['('].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules[')'].isValid = true;
   singleSymbolSyntaxRules[')'].tokenType = Token_Type_Paren_Right;
   singleSymbolSyntaxRules[')'].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules['['].isValid = true;
   singleSymbolSyntaxRules['['].tokenType = Token_Type_SquareBrace_Left;
   singleSymbolSyntaxRules['['].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules[']'].isValid = true;
   singleSymbolSyntaxRules[']'].tokenType = Token_Type_SquareBrace_Right;
   singleSymbolSyntaxRules[']'].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules['{'].isValid = true;
   singleSymbolSyntaxRules['{'].tokenType = Token_Type_CurlyBrace_Left;
   singleSymbolSyntaxRules['{'].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules['}'].isValid = true;
   singleSymbolSyntaxRules['}'].tokenType = Token_Type_CurlyBrace_Right;
   singleSymbolSyntaxRules['}'].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules[','].isValid = true;
   singleSymbolSyntaxRules[','].tokenType = Token_Type_Comma;
   singleSymbolSyntaxRules[','].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules['.'].isValid = true;
   singleSymbolSyntaxRules['.'].tokenType = Token_Type_Dot;
   singleSymbolSyntaxRules['.'].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules['?'].isValid = true;
   singleSymbolSyntaxRules['?'].tokenType = Token_Type_Question;
   singleSymbolSyntaxRules['?'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['`'].isValid = true;
   singleSymbolSyntaxRules['`'].tokenType = Token_Type_Backtick;
   singleSymbolSyntaxRules['`'].canAppearNextToAnyToken = true;

   singleSymbolSyntaxRules['@'].isValid = true;
   singleSymbolSyntaxRules['@'].tokenType = Token_Type_Arroba;
   singleSymbolSyntaxRules['@'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['#'].isValid = true;
   singleSymbolSyntaxRules['#'].tokenType = Token_Type_Pound;
   singleSymbolSyntaxRules['#'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['$'].isValid = true;
   singleSymbolSyntaxRules['$'].tokenType = Token_Type_Dollar;
   singleSymbolSyntaxRules['$'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules[':'].isValid = true;
   singleSymbolSyntaxRules[':'].tokenType = Token_Type_Colon;
   singleSymbolSyntaxRules[':'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['-'].isValid = true;
   singleSymbolSyntaxRules['-'].tokenType = Token_Type_Dash;
   singleSymbolSyntaxRules['-'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['+'].isValid = true;
   singleSymbolSyntaxRules['+'].tokenType = Token_Type_Plus;
   singleSymbolSyntaxRules['+'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['/'].isValid = true;
   singleSymbolSyntaxRules['/'].tokenType = Token_Type_Slash;
   singleSymbolSyntaxRules['/'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['*'].isValid = true;
   singleSymbolSyntaxRules['*'].tokenType = Token_Type_Asterisk;
   singleSymbolSyntaxRules['*'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['='].isValid = true;
   singleSymbolSyntaxRules['='].tokenType = Token_Type_Equal;
   singleSymbolSyntaxRules['='].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['<'].isValid = true;
   singleSymbolSyntaxRules['<'].tokenType = Token_Type_AngleBracket_Left;
   singleSymbolSyntaxRules['<'].canAppearNextToAnyToken = false;

   singleSymbolSyntaxRules['>'].isValid = true;
   singleSymbolSyntaxRules['>'].tokenType = Token_Type_AngleBracket_Right;
   singleSymbolSyntaxRules['>'].canAppearNextToAnyToken = false;
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
   static Token_t singleCharacterSymbol; // Declared static to ensure initialized to zero
   singleCharacterSymbol.type = singleSymbolSyntaxRules[current].tokenType;
   singleCharacterSymbol.value = 0;

   List_Add(tokenList, &singleCharacterSymbol);
   AdvanceOne();
}

static void ReportUnknownSymbolError(char current, I_Error_t * errorHandler, I_List_t *tokenList)
{
   char message[19] = "Unknown symbol ' '";
   message[16] = current;
   Error_Report(errorHandler, message);

   Token_t unknown = { .type = Token_Type_Unknown, .value = 0 };
   List_Add(tokenList, &unknown);

   AdvanceOne();
}

static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokenList)
{
   REINTERPRET(instance, interface, Lexer_Concrete_t *);
   sourceStringCharacterPointer = source;

   FillSingleSymbolSyntaxRuleArray(); // Makes a big static table indexed by the characters themselves

   char current;
   while((current = Peek()) != '\0')
   {
      if(isspace(current))
      {
         AdvanceOne();
      }
      else if(isIdentifier(current))
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
         ReportUnknownSymbolError(current, instance->errorHandler, tokenList);
      }
   }
}

void Lexer_Concrete_Init(Lexer_Concrete_t *instance, I_Error_t *errorHandler)
{
   instance->interface.lex = &lex;

   instance->errorHandler = errorHandler;
}
