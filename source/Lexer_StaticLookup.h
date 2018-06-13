/***
 * File: Lexer_StaticLookup.h
 * Desc: Implementation of I_Lexer using a static jump table for each ASCII character
 */

#ifndef _LEXER_STATICLOOKUP_H
#define _LEXER_STATICLOOKUP_H

#include "I_Lexer.h"
#include "I_Error.h"
#include "Token.h"

typedef struct
{
   I_Lexer_t interface;

   I_Error_t *errorHandler;
   I_List_t *tokenList;
   Token_t token;
   const char *beginning;
   const char *current;
   size_t line;
} Lexer_StaticLookup_t;

/*
 * Initialize a Lexer_StaticLookup.
 */
void Lexer_StaticLookup_Init(Lexer_StaticLookup_t *instance, I_Error_t *errorHandler);

#endif
