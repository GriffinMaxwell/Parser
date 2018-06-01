/***
 * File: Token.h
 * Desc: Parser token type
 */

#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdint.h>

enum
{

   // Literals
   Token_Type_Identifier,
   Token_Type_String,
   Token_Type_Number,

   // Keywords
   Token_Type_,
   Token_Type_,
   Token_Type_,
   Token_Type_,
   Token_Type_,

   Token_Type_,

   // Unknown
   Token_Type_Unknown
}
typedef uint8_t Token_Type_t;

typedef struct
{
   Token_Type_t type;
} Token_t;

#endif
