/***
 * File: Lexer_StaticLookup.c
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "Lexer_StaticLookup.h"
#include "util.h"

enum
{
   Touchy_DontCare = 0,
   Touchy_Yes,
   Touchy_No
};
typedef uint8_t Touchiness_t;

typedef struct
{
   void (*what)(Lexer_StaticLookup_t *instance);
   Token_Type_t type;
   Touchiness_t touchiness;
   Token_Type_t digraphType;
} CharacterInfo_Entry_t;

/*********************************
* Forward declarations because of circular calls between table and functions
*********************************/
static void Colon(Lexer_StaticLookup_t *instance);
static void Dash(Lexer_StaticLookup_t *instance);
static void DigraphOrSymbol(Lexer_StaticLookup_t *instance);
static void Dot(Lexer_StaticLookup_t *instance);
static void Exclamation(Lexer_StaticLookup_t *instance);
static void Identifier(Lexer_StaticLookup_t *instance);
static void Ignore(Lexer_StaticLookup_t *instance);
static void IncrementLineCounter(Lexer_StaticLookup_t *instance);
static void NumberLiteralOrIdentifier(Lexer_StaticLookup_t *instance);
static void Pound(Lexer_StaticLookup_t *instance);
static void ReportUnexpectedCharacter(Lexer_StaticLookup_t *instance);
static void StringLiteral(Lexer_StaticLookup_t *instance);
static void Symbol(Lexer_StaticLookup_t *instance);
static void SymbolicLiteral(Lexer_StaticLookup_t *instance);
static void Tilde(Lexer_StaticLookup_t *instance);

/*********************************
 * Movement through source string
 *********************************/
static inline char Peek(Lexer_StaticLookup_t *instance)
{
   return *instance->current;
}

static inline char PeekNext(Lexer_StaticLookup_t *instance)
{
   return (instance->current[1] == '\0') ? ' ' : instance->current[1];
}

static inline char PeekPrevious(Lexer_StaticLookup_t *instance)
{
   return (instance->current == instance->beginning) ? ' ' : *(instance->current - 1);
}

static inline char PeekAhead(Lexer_StaticLookup_t *instance, size_t ahead)
{
   return (instance->current[ahead] == '\0') ? ' ' : instance->current[ahead];
}

static inline void AdvanceOne(Lexer_StaticLookup_t *instance)
{
   instance->current++;
}

static inline void AdvanceMany(Lexer_StaticLookup_t *instance, size_t many)
{
   instance->current += many;
}

static void AddToken(Lexer_StaticLookup_t *instance, Token_Type_t type, const char *lexeme, size_t length, size_t line)
{
   instance->token.type = type;
   instance->token.lexeme = lexeme;
   instance->token.length = length;
   instance->token.line = line;

   List_Add(instance->tokenList, &instance->token);
}

static const CharacterInfo_Entry_t characterInfoTable[128] =
{
   { .what = ReportUnexpectedCharacter }, // null
   { .what = ReportUnexpectedCharacter }, // start of heading
   { .what = ReportUnexpectedCharacter }, // start of text
   { .what = ReportUnexpectedCharacter }, // end of text
   { .what = ReportUnexpectedCharacter }, // end of transmission
   { .what = ReportUnexpectedCharacter }, // enquiry
   { .what = ReportUnexpectedCharacter }, // ACK
   { .what = ReportUnexpectedCharacter }, // bell
   { .what = ReportUnexpectedCharacter }, // backspace
   { .what = Ignore                    }, // horizontal tab
   { .what = IncrementLineCounter      }, // LF
   { .what = Ignore                    }, // vertical tab
   { .what = Ignore                    }, // FF
   { .what = Ignore                    }, // CR
   { .what = ReportUnexpectedCharacter }, // shift out
   { .what = ReportUnexpectedCharacter }, // shift in
   { .what = ReportUnexpectedCharacter }, // data link escape
   { .what = ReportUnexpectedCharacter }, // device control 1
   { .what = ReportUnexpectedCharacter }, // device control 2
   { .what = ReportUnexpectedCharacter }, // device control 3
   { .what = ReportUnexpectedCharacter }, // device control 4
   { .what = ReportUnexpectedCharacter }, // NAK
   { .what = ReportUnexpectedCharacter }, // SYN
   { .what = ReportUnexpectedCharacter }, // end of transmission block
   { .what = ReportUnexpectedCharacter }, // cancel
   { .what = ReportUnexpectedCharacter }, // end of medium
   { .what = ReportUnexpectedCharacter }, // substitute
   { .what = ReportUnexpectedCharacter }, // escape
   { .what = ReportUnexpectedCharacter }, // file separator
   { .what = ReportUnexpectedCharacter }, // group separator
   { .what = ReportUnexpectedCharacter }, // record separator
   { .what = ReportUnexpectedCharacter }, // unit separator

   { .what = Ignore                                                                                     }, // Space
   { .what = Exclamation,               .type = Token_Type_Unused,             .touchiness = Touchy_Yes, .digraphType = Token_Type_BangEqual }, // !
   { .what = StringLiteral,                                                    .touchiness = Touchy_Yes }, // "
   { .what = Pound,                     .type = Token_Type_Pound,              .touchiness = Touchy_Yes }, // #
   { .what = Symbol,                    .type = Token_Type_Dollar,             .touchiness = Touchy_Yes }, // $
   { .what = ReportUnexpectedCharacter                                                                  }, // %
   { .what = ReportUnexpectedCharacter                                                                  }, // &
   { .what = ReportUnexpectedCharacter                                                                  }, // '
   { .what = Symbol,                    .type = Token_Type_Paren_Left,         .touchiness = Touchy_No  }, // (
   { .what = Symbol,                    .type = Token_Type_Paren_Right,        .touchiness = Touchy_No  }, // )
   { .what = Symbol,                    .type = Token_Type_Asterisk,           .touchiness = Touchy_Yes }, // *
   { .what = Symbol,                    .type = Token_Type_Plus,               .touchiness = Touchy_Yes }, // +
   { .what = Symbol,                    .type = Token_Type_Comma,              .touchiness = Touchy_No  }, // ,
   { .what = Dash,                      .type = Token_Type_Dash,               .touchiness = Touchy_Yes }, // -
   { .what = Dot,                                                              .touchiness = Touchy_No  }, // .
   { .what = Symbol,                    .type = Token_Type_Slash,              .touchiness = Touchy_Yes }, // /
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 0
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 1
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 2
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 3
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 4
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 5
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 6
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 7
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 8
   { .what = NumberLiteralOrIdentifier,                                        .touchiness = Touchy_Yes }, // 9
   { .what = Colon,                                                            .touchiness = Touchy_No  }, // :
   { .what = ReportUnexpectedCharacter                                                                  }, // ;
   { .what = DigraphOrSymbol,           .type = Token_Type_AngleBracket_Left,  .touchiness = Touchy_Yes, .digraphType = Token_Type_LessEqual }, // <
   { .what = DigraphOrSymbol,           .type = Token_Type_Equal,              .touchiness = Touchy_Yes, .digraphType = Token_Type_EqualEqual }, // =
   { .what = DigraphOrSymbol,           .type = Token_Type_AngleBracket_Right, .touchiness = Touchy_Yes, .digraphType = Token_Type_GreaterEqual }, // >
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // ?

   { .what = Symbol,                    .type = Token_Type_Arroba,             .touchiness = Touchy_Yes }, // @
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // A
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // B
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // C
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // D
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // E
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // F
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // G
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // H
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // I
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // J
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // K
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // L
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // M
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // N
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // O
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // P
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // Q
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // R
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // S
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // T
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // U
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // V
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // W
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // X
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // Y
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // Z
   { .what = Symbol,                    .type = Token_Type_SquareBrace_Left,   .touchiness = Touchy_No  }, // [
   { .what = ReportUnexpectedCharacter                                                                  }, // backslash
   { .what = Symbol,                    .type = Token_Type_SquareBrace_Right,  .touchiness = Touchy_No  }, // ]
   { .what = ReportUnexpectedCharacter                                                                  }, // ^
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // _

   { .what = Symbol,                    .type = Token_Type_Backtick                                     }, // `
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // a
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // b
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // c
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // d
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // e
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // f
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // g
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // h
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // i
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // j
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // k
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // l
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // m
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // n
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // o
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // p
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // q
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // r
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // s
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // t
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // u
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // v
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // w
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // x
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // y
   { .what = Identifier,                                                       .touchiness = Touchy_Yes }, // z
   { .what = Symbol,                    .type = Token_Type_CurlyBrace_Left,    .touchiness = Touchy_No  }, // {
   { .what = ReportUnexpectedCharacter                                                                  }, // |
   { .what = Symbol,                    .type = Token_Type_CurlyBrace_Right,   .touchiness = Touchy_No  }, // }
   { .what = Tilde,                                                            .touchiness = Touchy_Yes }, // ~
   { .what = ReportUnexpectedCharacter                                                                  } // DEL
};

/*********************************
 * Actions
 *********************************/
static void Ignore(Lexer_StaticLookup_t *instance)
{
   AdvanceOne(instance);
}

static void IncrementLineCounter(Lexer_StaticLookup_t *instance)
{
   instance->line++;
   AdvanceOne(instance);
}

static void ReportUnexpectedCharacter(Lexer_StaticLookup_t *instance)
{
   if(iscntrl(Peek(instance)))
   {
      Error_Report(instance->errorHandler, instance->line, "Unexpected non-printable character");
   }
   else
   {
      char message[25] = "Unexpected character ' '";
      message[22] = Peek(instance);
      Error_Report(instance->errorHandler, instance->line, message);
   }

   AdvanceOne(instance);
}

static void Identifier(Lexer_StaticLookup_t *instance)
{
   const char *beginning = instance->current;
   size_t length = 0;
   bool validIdentifier = false;

   while(isalpha(Peek(instance)) || Peek(instance) == '_' || Peek(instance) == '-'
         || Peek(instance) == '#' || Peek(instance) == '!' || Peek(instance) == '?')
   {
      validIdentifier = validIdentifier || (isalpha(Peek(instance)) || Peek(instance) == '?');

      AdvanceOne(instance);
      length++;
   }

   if(validIdentifier)
   {
      AddToken(instance, Token_Type_Identifier, beginning, length, instance->line);
   }
   else
   {
      char message[38+length];
      sprintf(message, "Identifier name missing [a-zA-Z?]: '%.*s'", (int)length, beginning);
      Error_Report(instance->errorHandler, instance->line, message);
   }
}

static void CheckSpacing(Lexer_StaticLookup_t *instance, uint8_t length)
{
   char message[66];
   bool touchyOnLeft = characterInfoTable[PeekPrevious(instance)].touchiness == Touchy_Yes;
   bool touchyOnRight = characterInfoTable[PeekAhead(instance, length)].touchiness == Touchy_Yes;

   if(touchyOnLeft && touchyOnRight)
   {
      sprintf(message, "\"Touchy\" symbol '%.*s' next to other touchy symbols '%c' and '%c'",  length, instance->current, PeekPrevious(instance), PeekAhead(instance, length));
      Error_Report(instance->errorHandler, instance->line, message);
   }
   else if(touchyOnLeft)
   {
      sprintf(message, "\"Touchy\" symbol '%.*s' next to another touchy symbol '%c'",  length, instance->current, PeekPrevious(instance));
      Error_Report(instance->errorHandler, instance->line, message);
   }
   else if(touchyOnRight)
   {
      sprintf(message, "\"Touchy\" symbol '%.*s' next to another touchy symbol '%c'",  length, instance->current, PeekAhead(instance, length));
      Error_Report(instance->errorHandler, instance->line, message);
   }
}

static void WideSymbol(Lexer_StaticLookup_t *instance, uint8_t width, Token_Type_t type, Touchiness_t touchiness)
{
   if(touchiness == Touchy_Yes)
   {
      CheckSpacing(instance, width);
   }

   AddToken(instance, type, instance->current, width, instance->line);
   AdvanceMany(instance, width);
}

static void Symbol(Lexer_StaticLookup_t *instance)
{
   WideSymbol(instance, 1, characterInfoTable[Peek(instance)].type, characterInfoTable[Peek(instance)].touchiness);
}

static void DigraphOrSymbol(Lexer_StaticLookup_t *instance)
{
   if(PeekNext(instance) == '=')
   {
      WideSymbol(instance, 2, characterInfoTable[Peek(instance)].digraphType, Touchy_Yes);
   }
   else
   {
      WideSymbol(instance, 1, characterInfoTable[Peek(instance)].type, characterInfoTable[Peek(instance)].touchiness);
   }
}

static void NumberLiteralOrIdentifier(Lexer_StaticLookup_t *instance)
{
   Token_Type_t type = Token_Type_Literal_Number;
   const char *beginning = instance->current;
   size_t length = 1;

   bool containsDecimalPoint = (Peek(instance) == '.');
   AdvanceOne(instance);

   while(isdigit(Peek(instance)) || (!containsDecimalPoint && (Peek(instance) == '.')))
   {
      containsDecimalPoint = containsDecimalPoint || Peek(instance) == '.';
      AdvanceOne(instance);
      length++;
   }

   if(!containsDecimalPoint && (PeekNext(instance) == '\'' || PeekNext(instance) == '"'))
   {
      type = Token_Type_Identifier;
      AdvanceOne(instance);
      length++;
   }

   AddToken(instance, type, beginning, length, instance->line);
}

static void StringLiteral(Lexer_StaticLookup_t *instance)
{
   const char *beginning = instance->current;
   size_t line = instance->line;
   size_t length = 1;

   AdvanceOne(instance);   // Past opening "
   while(Peek(instance) != '"')
   {
      if(Peek(instance) == '\0')
      {
         Error_Report(instance->errorHandler, instance->line, "String literal missing ending \"");
         return;
      }
      if(Peek(instance) == '\n')
      {
         Error_Report(instance->errorHandler, instance->line, "String literal not contained on one line.");
         instance->line++;
      }

      AdvanceOne(instance);
      length++;
   }

   AdvanceOne(instance);   // Past closing "
   length++;

   AddToken(instance, Token_Type_Literal_String, beginning, length, instance->line);
}

static void Exclamation(Lexer_StaticLookup_t *instance)
{
   if(PeekNext(instance) == '=')
   {
      WideSymbol(instance, 2, characterInfoTable['!'].digraphType, Touchy_Yes);
   }
   else
   {
      Identifier(instance);
   }
}

static void Pound(Lexer_StaticLookup_t *instance)
{
   if(isalpha(PeekNext(instance))
      || PeekNext(instance) == '_'
      || PeekNext(instance) == '-'
      || PeekNext(instance) == '#'
      || PeekNext(instance) == '!'
      || PeekNext(instance) == '?')
   {
      Identifier(instance);
   }
   else
   {
      WideSymbol(instance, 1, Token_Type_Pound, characterInfoTable['#'].touchiness);
   }
}

static void Dot(Lexer_StaticLookup_t *instance)
{
   if(isdigit(PeekNext(instance)))
   {
      if(PeekPrevious(instance) != ' ')
      {
         Error_Report(instance->errorHandler, instance->line, "Missing space before decimal number with no leading zero");
      }

      NumberLiteralOrIdentifier(instance);
   }
   else if(PeekNext(instance) == '.')
   {
      if(PeekAhead(instance, 2) == '.')
      {
         WideSymbol(instance, 3, Token_Type_DotDotDot, characterInfoTable['.'].touchiness);
      }
      else
      {
         WideSymbol(instance, 2, Token_Type_DotDot, characterInfoTable['.'].touchiness);
      }
   }
   else
   {
      WideSymbol(instance, 1, Token_Type_Dot, characterInfoTable['.'].touchiness);
   }
}

static void Colon(Lexer_StaticLookup_t *instance)
{
   if(isalpha(PeekNext(instance))
      || PeekNext(instance) == '_'
      || PeekNext(instance) == '-'
      || PeekNext(instance) == '#'
      || PeekNext(instance) == '!'
      || PeekNext(instance) == '?')
   {
      SymbolicLiteral(instance);
   }
   else
   {
      // Colon is allowed to touch on left as long as next is a space
      if(isspace(PeekNext(instance)))
      {
         WideSymbol(instance, 1, Token_Type_Colon, Touchy_No);
      }
      else
      {
         Error_Report(instance->errorHandler, instance->line, "Missing space after ':'");
      }
   }
}

static void Dash(Lexer_StaticLookup_t *instance)
{
   if(isalpha(PeekNext(instance))
      || PeekNext(instance) == '_'
      || PeekNext(instance) == '-'
      || PeekNext(instance) == '#'
      || PeekNext(instance) == '!'
      || PeekNext(instance) == '?')
   {
      Identifier(instance);
   }
   else
   {
      WideSymbol(instance, 1, Token_Type_Dash, characterInfoTable['-'].touchiness);
   }
}

static void Tilde(Lexer_StaticLookup_t *instance)
{
   WideSymbol(instance, 1, Token_Type_Identifier, characterInfoTable['~'].touchiness);
}

// TODO: merge overlap with literal function so there isn't as much copied code
static void SymbolicLiteral(Lexer_StaticLookup_t *instance)
{
   const char *beginning = instance->current;
   size_t length = 1;
   bool validSymbolic = false;

   AdvanceOne(instance); // Past :

   while(isalpha(Peek(instance)) || Peek(instance) == '_' || Peek(instance) == '-'
         || Peek(instance) == '#' || Peek(instance) == '!' || Peek(instance) == '?')
   {
      validSymbolic = validSymbolic || (isalpha(Peek(instance)) || Peek(instance) == '?');

      AdvanceOne(instance);
      length++;
   }

   if(validSymbolic)
   {
      AddToken(instance, Token_Type_Literal_Symbol, beginning, length, instance->line);
   }
   else
   {
      char message[34+length];
      sprintf(message, "Symbol name missing [a-zA-Z?]: '%.*s'", (int)length, beginning);
      Error_Report(instance->errorHandler, instance->line, message);
   }
}

/*********************************
 * Top-level functions
 *********************************/
static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokenList)
{
   REINTERPRET(instance, interface, Lexer_StaticLookup_t *);
   instance->beginning = source;
   instance->current = source;
   instance->tokenList = tokenList;
   instance->line = 1;

   while(Peek(instance) != '\0')
   {
      if(Peek(instance) > 0)
      {
         characterInfoTable[Peek(instance)].what(instance);
      }
      else
      {
         Error_Report(instance->errorHandler, instance->line, "Unexpected non-ascii character");
         AdvanceOne(instance);
      }
   }
}

void Lexer_StaticLookup_Init(Lexer_StaticLookup_t *instance, I_Error_t *errorHandler)
{
   instance->interface.lex = &lex;
   instance->errorHandler = errorHandler;
}
