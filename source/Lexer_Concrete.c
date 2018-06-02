/***
 * File: Lexer_Concrete.c
 */

#include "Lexer_Concrete.h"

static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokens)
{

}

void Lexer_Concrete_Init(Lexer_Concrete_t *instance)
{
   instance->interface.lex = &lex;
}
