/***
 * File: I_Parser.h
 * Desc: Interface functions for Parsing a string into tokens
 */
#ifndef _I_PARSER_H
#define _I_PARSER_H


typedef struct I_Parser_t
{
   /*
    * Parse a string into a list of tokens.
    *
    * @param tokens - storage for the tokens
    * @pre - tokens is an empty list
    */
   void (*parse)(struct I_Parser_t *interface, I_List_t *tokens);
} I_List_t;

#define Parser_Parse(interface, tokens) \
   (interface)->parse((interface), (tokens))

#endif
