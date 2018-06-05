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
   Token_Type_t tokenType;
   bool canAppearNextToAnyToken;
   bool isSymbol;
   bool isIdentifierCharacter;
} symbolSyntaxRules[128] = { 0 };

static const char *sourceStringCharacterPointer;
static Token_t newToken;

static void FillSymbolSyntaxRuleArray()
{
   symbolSyntaxRules['('].tokenType = Token_Type_Paren_Left;
   symbolSyntaxRules['('].canAppearNextToAnyToken = true;
   symbolSyntaxRules['('].isSymbol = true;
   symbolSyntaxRules['('].isIdentifierCharacter = false;

   symbolSyntaxRules[')'].tokenType = Token_Type_Paren_Right;
   symbolSyntaxRules[')'].canAppearNextToAnyToken = true;
   symbolSyntaxRules[')'].isSymbol = true;
   symbolSyntaxRules[')'].isIdentifierCharacter = false;

   symbolSyntaxRules['['].tokenType = Token_Type_SquareBrace_Left;
   symbolSyntaxRules['['].canAppearNextToAnyToken = true;
   symbolSyntaxRules['['].isSymbol = true;
   symbolSyntaxRules['['].isIdentifierCharacter = false;

   symbolSyntaxRules[']'].tokenType = Token_Type_SquareBrace_Right;
   symbolSyntaxRules[']'].canAppearNextToAnyToken = true;
   symbolSyntaxRules[']'].isSymbol = true;
   symbolSyntaxRules[']'].isIdentifierCharacter = false;

   symbolSyntaxRules['{'].tokenType = Token_Type_CurlyBrace_Left;
   symbolSyntaxRules['{'].canAppearNextToAnyToken = true;
   symbolSyntaxRules['{'].isSymbol = true;
   symbolSyntaxRules['{'].isIdentifierCharacter = false;

   symbolSyntaxRules['}'].tokenType = Token_Type_CurlyBrace_Right;
   symbolSyntaxRules['}'].canAppearNextToAnyToken = true;
   symbolSyntaxRules['}'].isSymbol = true;
   symbolSyntaxRules['}'].isIdentifierCharacter = false;

   symbolSyntaxRules[','].tokenType = Token_Type_Comma;
   symbolSyntaxRules[','].canAppearNextToAnyToken = true;
   symbolSyntaxRules[','].isSymbol = true;
   symbolSyntaxRules[','].isIdentifierCharacter = false;

   symbolSyntaxRules['.'].tokenType = Token_Type_Dot;
   symbolSyntaxRules['.'].canAppearNextToAnyToken = true;
   symbolSyntaxRules['.'].isSymbol = true;
   symbolSyntaxRules['.'].isIdentifierCharacter = false;

   symbolSyntaxRules['`'].tokenType = Token_Type_Backtick;
   symbolSyntaxRules['`'].canAppearNextToAnyToken = true;
   symbolSyntaxRules['`'].isSymbol = true;
   symbolSyntaxRules['`'].isIdentifierCharacter = false;

   symbolSyntaxRules['@'].tokenType = Token_Type_Arroba;
   symbolSyntaxRules['@'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['@'].isSymbol = true;
   symbolSyntaxRules['@'].isIdentifierCharacter = false;

   symbolSyntaxRules['#'].tokenType = Token_Type_Pound;
   symbolSyntaxRules['#'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['#'].isSymbol = true;
   symbolSyntaxRules['#'].isIdentifierCharacter = true;

   symbolSyntaxRules['$'].tokenType = Token_Type_Dollar;
   symbolSyntaxRules['$'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['$'].isSymbol = true;
   symbolSyntaxRules['$'].isIdentifierCharacter = false;

   symbolSyntaxRules[':'].tokenType = Token_Type_Colon;
   symbolSyntaxRules[':'].canAppearNextToAnyToken = false;
   symbolSyntaxRules[':'].isSymbol = true;
   symbolSyntaxRules[':'].isIdentifierCharacter = false;

   symbolSyntaxRules['-'].tokenType = Token_Type_Dash;
   symbolSyntaxRules['-'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['-'].isSymbol = true;
   symbolSyntaxRules['-'].isIdentifierCharacter = true;

   symbolSyntaxRules['+'].tokenType = Token_Type_Plus;
   symbolSyntaxRules['+'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['+'].isSymbol = true;
   symbolSyntaxRules['+'].isIdentifierCharacter = false;

   symbolSyntaxRules['/'].tokenType = Token_Type_Slash;
   symbolSyntaxRules['/'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['/'].isSymbol = true;
   symbolSyntaxRules['/'].isIdentifierCharacter = false;

   symbolSyntaxRules['*'].tokenType = Token_Type_Asterisk;
   symbolSyntaxRules['*'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['*'].isSymbol = true;
   symbolSyntaxRules['*'].isIdentifierCharacter = false;

   symbolSyntaxRules['='].tokenType = Token_Type_Equal;
   symbolSyntaxRules['='].canAppearNextToAnyToken = false;
   symbolSyntaxRules['='].isSymbol = true;
   symbolSyntaxRules['='].isIdentifierCharacter = false;

   symbolSyntaxRules['<'].tokenType = Token_Type_AngleBracket_Left;
   symbolSyntaxRules['<'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['<'].isSymbol = true;
   symbolSyntaxRules['<'].isIdentifierCharacter = false;

   symbolSyntaxRules['>'].tokenType = Token_Type_AngleBracket_Right;
   symbolSyntaxRules['>'].canAppearNextToAnyToken = false;
   symbolSyntaxRules['>'].isSymbol = true;
   symbolSyntaxRules['>'].isIdentifierCharacter = false;
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

static inline char PeekAhead(size_t ahead)
{
   return sourceStringCharacterPointer[ahead];
}

static bool isIdentifier(char current)
{
   if(isalpha(current) || (current == '?'))
   {
      return true;
   }
   else if(symbolSyntaxRules[current].isIdentifierCharacter)
   {
      // Keep peeking ahead until you find an alpha character.
      // If you hit a space, null terminator, or non-identifier, can't be an identifier
      size_t i = 0;
      char next = current;
      while(next != '\0' || !isspace(next))
      {
         if(isalpha(next))
         {
            return true;
         }
         else if(symbolSyntaxRules[next].isIdentifierCharacter)
         {
            i++;
            next = PeekAhead(i);
         }
         else
         {
            return false;
         }
      }
   }

   // Kludgy rules for non-alpha characters that can appear in an identifier,
   // including the question mark, which can appear more than once in a row

}

static bool isMultiCharacterComparator(char current)
{
   return ((current == '<'
         || current == '>'
         || current == '!'
         || current == '=')
      && PeekNext() == '=');
}

static bool isMultiDot(char current)
{
   return (current == '.') && (PeekNext() == '.');
}

static bool isSingleCharacterSymbol(char current)
{
   return symbolSyntaxRules[current].isSymbol;  // Multi character symbols already been eliminated
}

static void ConsumeSingleCharacterSymbol(char current, I_List_t *tokenList)
{
   newToken.type = symbolSyntaxRules[current].tokenType;
   newToken.value = 0;

   List_Add(tokenList, &newToken);
   AdvanceOne();
}

static void ConsumeMultiCharacterComparator(char current, I_List_t *tokenList)
{
   switch(current)
   {
      case '<':
         newToken.type = Token_Type_LessEqual;
         newToken.value = 0;
         break;
      case '>':
         newToken.type = Token_Type_GreaterEqual;
         newToken.value = 0;
         break;
      case '!':
         newToken.type = Token_Type_BangEqual;
         newToken.value = 0;
         break;
      case '=':
         newToken.type = Token_Type_EqualEqual;
         newToken.value = 0;
         break;
   }

   List_Add(tokenList, &newToken);
   AdvanceOne();
   AdvanceOne();
}

static void ConsumeMultiDot(char current, I_List_t *tokenList)
{
   if (PeekAhead(2) == '.')
   {
      newToken.type = Token_Type_DotDotDot;
      AdvanceOne();
      AdvanceOne();
      AdvanceOne();
   }
   else
   {
      newToken.type = Token_Type_DotDot;
      AdvanceOne();
      AdvanceOne();
   }

   newToken.value = 0;
   List_Add(tokenList, &newToken);
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

   FillSymbolSyntaxRuleArray(); // Makes a big static table indexed by the characters themselves

   char current;
   while((current = Peek()) != '\0')
   {
      if(isspace(current))
      {
         AdvanceOne();  // Ignore whitespace
      }
      else if(isIdentifier(current))
      {

         Error_Report(instance->errorHandler, "Identifier");
         // ConsumeIdentifier();
      }
      else if(isMultiCharacterComparator(current))
      {
         ConsumeMultiCharacterComparator(current, tokenList);
      }
      else if(isMultiDot(current))
      {
         ConsumeMultiDot(current, tokenList);
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
