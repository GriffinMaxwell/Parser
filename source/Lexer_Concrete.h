/***
 * File: Lexer_Concrete.h
 * Desc: Implementation of I_Lexer
 */

#ifndef _LEXER_CONCRETE_H
#define _LEXER_CONCRETE_H

#include "I_Lexer.h"
#include "I_Error.h"

typedef struct
{
   I_Lexer_t interface;

   I_Error_t *errorHandler;
} Lexer_Concrete_t;

/*
 * Initialize a Lexer_Concrete.
 */
void Lexer_Concrete_Init(Lexer_Concrete_t *instance, I_Error_t *errorHandler);

#endif
