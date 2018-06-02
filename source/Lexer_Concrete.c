/***
 * File: Lexer_Concrete.c
 */

#include "Lexer_Concrete.h"
#include "util.h"

static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokens)
{
   REINTERPRET(instance, interface, Lexer_Concrete_t *);

   Error_Report(instance->errorHandler, "Lexer doesn't do anything yet.");
}

void Lexer_Concrete_Init(Lexer_Concrete_t *instance, I_Error_t *errorHandler)
{
   instance->interface.lex = &lex;

   instance->errorHandler = errorHandler;
}
