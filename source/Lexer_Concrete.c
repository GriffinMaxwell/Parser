/***
 * File: Lexer_Concrete.c
 */

#include <stdbool.h>
#include <stdio.h>
#include "Lexer_Concrete.h"
#include "Token.h"
#include "util.h"

static Token_t newToken;
static bool tokenFound;

static void TokenFound(Token_Type_t type)
{
   newToken.type = type;
   newToken.value = 0;

   tokenFound = true;
}

static void TokenFoundIfSurroundedByWhiteSpace(Token_Type_t type)
{
   tokenFound = false;
}

static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokenList)
{

   // Check identifier or literal (peek ahead and consume if necessary)
   // Check multi-character symbols (peek ahead if necessary)
   // Check single character symbols
   // Else unknown
   size_t i = 0;
   while(source[i] != '\0')
   {
      tokenFound = false;

      printf("%c\n", source[i]);
      switch(source[i])
      {
         case '(':
            TokenFound(Token_Type_Paren_Left);
            break;
         case ')':
            TokenFound(Token_Type_Paren_Right);
            break;
         case '[':
            TokenFound(Token_Type_SquareBrace_Left);
            break;
         case ']':
            TokenFound(Token_Type_SquareBrace_Right);
            break;
         case '{':
            TokenFound(Token_Type_CurlyBrace_Left);
            break;
         case '}':
            TokenFound(Token_Type_CurlyBrace_Right);
            break;
         case ',':
            TokenFound(Token_Type_Comma);
            break;
         case '.':
            // TokenFound(Token_Type_Dot);
            break;
         case '`':
            TokenFound(Token_Type_Backtick);
            break;
         case '@':
            TokenFound(Token_Type_Arroba);
            break;
         case '#':
            TokenFound(Token_Type_Pound);
            break;
         case '$':
            TokenFound(Token_Type_Dollar);
            break;
         case ':':
            TokenFound(Token_Type_Colon);
            break;
         default:
            TokenFound(Token_Type_Unknown);
            break;
      }

      if(tokenFound)
      {
         List_Add(tokenList, &newToken);
      }

      i++;
   }

   // Error_Report(instance->errorHandler, "Lexer doesn't do anything yet.");
}

void Lexer_Concrete_Init(Lexer_Concrete_t *instance, I_Error_t *errorHandler)
{
   instance->interface.lex = &lex;

   instance->errorHandler = errorHandler;
}
