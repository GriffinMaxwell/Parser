/***
 * File: Token.h
 * Desc: Parser token type
 */

#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdint.h>

enum
{
   Token_Type_Unused,

   Token_Type_AngleBracket_Left,
   Token_Type_AngleBracket_Right,
   Token_Type_Asterisk,
   Token_Type_Arroba,
   Token_Type_Backtick,
   Token_Type_BangEqual,
   Token_Type_Colon,
   Token_Type_Comma,
   Token_Type_CurlyBrace_Left,
   Token_Type_CurlyBrace_Right,
   Token_Type_Dash,
   Token_Type_Dollar,
   Token_Type_Dot,
   Token_Type_DotDot,
   Token_Type_DotDotDot,
   Token_Type_Equal,
   Token_Type_EqualEqual,
   Token_Type_GreaterEqual,
   Token_Type_Identifier,
   Token_Type_Literal_String,
   Token_Type_Literal_Number,
   Token_Type_Literal_Symbol,
   Token_Type_LessEqual,
   Token_Type_Paren_Left,
   Token_Type_Paren_Right,
   Token_Type_Plus,
   Token_Type_Pound,
   Token_Type_Slash,
   Token_Type_SquareBrace_Left,
   Token_Type_SquareBrace_Right
};
typedef uint8_t Token_Type_t;

typedef struct
{
   Token_Type_t type;
   const char *lexeme;
   size_t length;
   size_t line;
} Token_t;

#endif
