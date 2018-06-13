/***
 * File: I_Lexer.h
 * Desc: Interface functions for lexing a string into tokens
 */
#ifndef _I_LEXER_H
#define _I_LEXER_H

#include "I_List.h"

typedef struct I_Lexer_t
{
   /*
    * Lex a string into a list of tokens.
    *
    * @param source - string of source code characters
    * @param tokens - storage for the tokens
    * @pre - tokens is an empty list
    * @pre - source points to a valid string
    * @post - tokens that contain strings will point directly to source,
    *          so deallocating source will invalidate the tokens
    */
   void (*lex)(struct I_Lexer_t *interface, const char *source, I_List_t *tokens);
} I_Lexer_t;

#define Lexer_Lex(interface, source, tokens) \
   (interface)->lex((interface), (source), (tokens))

#endif
