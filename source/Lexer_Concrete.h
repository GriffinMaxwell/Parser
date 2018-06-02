/***
 * File: Lexer_Concrete.h
 * Desc: Implementation of I_Lexer
 */

#ifndef _LEXER_CONCRETE_H
#define _LEXER_CONCRETE_H

#include "I_Lexer.h"

typedef struct
{
   I_Lexer_t interface;
} Lexer_Concrete_t;

/*
 * Initialize a Lexer_Concrete.
 */
void Lexer_Concrete_Init(Lexer_Concrete_t *instance);

#endif
