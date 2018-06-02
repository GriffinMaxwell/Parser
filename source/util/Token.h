/***
 * File: Token.h
 * Desc: Parser token type
 */

#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdint.h>

enum
{
   // Word-like things: identifiers & literals
   Token_Type_Identifier_Unknown,   // [#_^-!a-zA-Z]*[a-zA-Z][#_^-!a-zA-Z]* -- at least 1 letter and 1 or more special characters
   Token_Type_Identifier_Phrasal,   // For now, don't handle phrasal functions
   Token_Type_Identifier_Keyword,   // For now, don't distinguish keywords until parse step
   Token_Type_Identifier_Type,      // For now, don't distinguish keywords until parse step
   Token_Type_Identifier_Variable,  // For now, don't distinguish keywords until parse step
   Token_Type_Literal_String,       // "Only double quote Strings allowed for now"
   Token_Type_Literal_Number,       // [0-9]
   Token_Type_Literal_Symbol,       // :identifier

   // Single Character Symbols
   Token_Type_Paren_Left,
   Token_Type_Paren_Right,
   Token_Type_SquareBrace_Left,
   Token_Type_SquareBrace_Right,
   Token_Type_CurlyBrace_Left,
   Token_Type_CurlyBrace_Right,
   Token_Type_Comma,
   Token_Type_Dot,   // can appear +1 times in a row ...
   Token_Type_Question, // can tack onto identifiers -- can appear +1 times in a row ???
   Token_Type_Backtick,
   Token_Type_Arroba,
   Token_Type_Pound, // can tack onto identifiers -- Must be surrounded by whitespace or paren or curly brace
   Token_Type_Dollar, // Must be surrounded by whitespace or paren or curly brace
   Token_Type_Colon, // can tack onto identifiers
   Token_Type_Dash, // can tack onto identifiers? -- Must be surrounded by whitespace or paren or curly brace
   Token_Type_Plus, // Must be surrounded by whitespace or paren or curly brace
   Token_Type_Slash, // Must be surrounded by whitespace or paren or curly brace
   Token_Type_Asterisk,  // Must be surrounded by whitespace or paren or curly brace
   Token_Type_Equal, // Must be surrounded by whitespace or paren or curly brace
   Token_Type_AngleBracket_Left, // Must be surrounded by whitespace or paren or curly brace
   Token_Type_AngleBracket_Right, // Must be surrounded by whitespace or paren or curly brace

   // Multi-character Symbols
   Token_Type_LessEqual,
   Token_Type_GreaterEqual,
   Token_Type_EqualEqual,
   Token_Type_BangEqual,
   Token_Type_DotDot,
   Token_Type_DotDotDot,

   // Unknown
   Token_Type_Unknown
};
typedef uint8_t Token_Type_t;

typedef struct
{
   Token_Type_t type;

   union
   {
      char *lexeme;
      long value;
   };
} Token_t;

#endif
