/***
 * File: Lexer_Concrete.c
 */

#include <stdbool.h>
#include <stdint.h>
#include "Lexer_Concrete.h"
#include "util.h"

typedef struct
{
   void (*action)(Lexer_Concrete_t *instance);
} Action_Table_t;

static inline char Peek(Lexer_Concrete_t *instance)
{
   return *instance->current;
}
static inline char PeekNext(Lexer_Concrete_t *instance)
{
   (instance->current[1] == '\0') ? ' ' : instance->current[1];
}

static inline char PeekPrevious(Lexer_Concrete_t *instance)
{
   (instance->current == instance->beginning) ? ' ' : *(instance->current - 1);
}

static inline char PeekAhead(Lexer_Concrete_t *instance, size_t ahead);

static inline void AdvanceOne(Lexer_Concrete_t *instance)
{
   instance->current++;
}

static bool canTouch(char c)
{
   return isspace(c)
      || c == '('
      || c == ')'
      || c == '['
      || c == ']'
      || c == '{'
      || c == '}'
      || c == ','
      || c == '`'
      || c == '.'
      || c == ':';
}

static void Ignore(Lexer_Concrete_t *instance)
{
   AdvanceOne(instance);
}

static void ReportUnknownSymbol(Lexer_Concrete_t *instance)
{

}

static void ReportImproperUseOfApos(Lexer_Concrete_t *instance)
{

}

static void ConsumeIdentifier(Lexer_Concrete_t *instance)
{
   instance->token.type = Token_Type_Identifier_Unknown;
   instance->token.lexeme = instance->current;

   bool foundLetter = false;
   while(isalpha(Peek(instance))
      || Peek(instance) == '_'
      || Peek(instance) == '-'
      || Peek(instance) == '#'
      || Peek(instance) == '!'
      || Peek(instance) == '?')
   {
      if(isalpha(Peek(instance)) || Peek(instance) == '?')
      {
         foundLetter = true;
      }
      AdvanceOne(instance);
   }

   if(foundLetter)
   {
      List_Add(instance->tokenList, &instance->token);
   }
   else
   {
      Error_Report(instance->errorHandler, "Bad identifier name.");
   }
}

static void ConsumeUnigraphSymbol(Lexer_Concrete_t *instance)
{
   //switch()
}

static void CheckSpacingAndConsumeUnigraphSymbol(Lexer_Concrete_t *instance)
{
   switch(Peek(instance))
   {
      case '@':
      case '$':
      case '+':
      case '/':
      case '*':
      case '=':
      case '<':
      case '>':
         if(!canTouch(PeekPrevious(instance)) || !canTouch(PeekNext(instance)))
         {
            char message[45] = "Symbol ' ' cannot appear next to ' ' or ' '";
            message[8] = Peek(instance);
            message[34] = PeekPrevious(instance);
            message[42] = PeekNext(instance);
            Error_Report(instance->errorHandler, message);
         }
   }
}

static void ConsumeNumberLiteral(Lexer_Concrete_t *instance)
{

}

static void ConsumeStringLiteral(Lexer_Concrete_t *instance)
{

}

static void CheckEqualsDigraph(Lexer_Concrete_t *instance)
{

}

static void CheckEqualsDigraphOrIdentifier(Lexer_Concrete_t *instance)
{

}

static void CheckNumberOrIdentifier(Lexer_Concrete_t *instance)
{

}

static void HandleSpecialCase_Pound(Lexer_Concrete_t *instance)
{

}

static void HandleSpecialCase_Dot(Lexer_Concrete_t *instance)
{

}

static void HandleSpecialCase_Colon(Lexer_Concrete_t *instance)
{

}

static void HandleSpecialCase_Dash(Lexer_Concrete_t *instance)
{

}

const Action_Table_t actionTable[128] =
{
   { ReportUnknownSymbol }, // null
   { ReportUnknownSymbol }, // start of heading
   { ReportUnknownSymbol }, // start of text
   { ReportUnknownSymbol }, // end of text
   { ReportUnknownSymbol }, // end of transmission
   { ReportUnknownSymbol }, // enquiry
   { ReportUnknownSymbol }, // ACK
   { ReportUnknownSymbol }, // bell
   { ReportUnknownSymbol }, // backspace
   { Ignore },              // horizontal tab
   { Ignore },              // LF
   { Ignore },              // vertical tab
   { Ignore },              // FF
   { Ignore },              // CR
   { ReportUnknownSymbol }, // shift out
   { ReportUnknownSymbol }, // shift in
   { ReportUnknownSymbol }, // data link escape
   { ReportUnknownSymbol }, // device control 1
   { ReportUnknownSymbol }, // device control 2
   { ReportUnknownSymbol }, // device control 3
   { ReportUnknownSymbol }, // device control 4
   { ReportUnknownSymbol }, // NAK
   { ReportUnknownSymbol }, // SYN
   { ReportUnknownSymbol }, // end of transmission block
   { ReportUnknownSymbol }, // cancel
   { ReportUnknownSymbol }, // end of medium
   { ReportUnknownSymbol }, // substitute
   { ReportUnknownSymbol }, // escape
   { ReportUnknownSymbol }, // file separator
   { ReportUnknownSymbol }, // group separator
   { ReportUnknownSymbol }, // record separator
   { ReportUnknownSymbol }, // unit separator

   { Ignore }, // Space
   { CheckEqualsDigraphOrIdentifier }, // !
   { ConsumeStringLiteral }, // "
   { HandleSpecialCase_Pound }, // #
   { ConsumeUnigraphSymbol }, // $
   { ReportUnknownSymbol }, // %
   { ReportUnknownSymbol }, // &
   { ReportImproperUseOfApos }, // '
   { ConsumeUnigraphSymbol }, // (
   { ConsumeUnigraphSymbol }, // )
   { CheckSpacingAndConsumeUnigraphSymbol }, // *
   { CheckSpacingAndConsumeUnigraphSymbol }, // +
   { ConsumeUnigraphSymbol }, // ,
   { HandleSpecialCase_Dash }, // -
   { ConsumeUnigraphSymbol }, // .
   { CheckSpacingAndConsumeUnigraphSymbol }, // /

   { CheckNumberOrIdentifier }, // 0
   { CheckNumberOrIdentifier }, // 1
   { CheckNumberOrIdentifier }, // 2
   { CheckNumberOrIdentifier }, // 3
   { CheckNumberOrIdentifier }, // 4
   { CheckNumberOrIdentifier }, // 5
   { CheckNumberOrIdentifier }, // 6
   { CheckNumberOrIdentifier }, // 7
   { CheckNumberOrIdentifier }, // 8
   { CheckNumberOrIdentifier }, // 9

   { HandleSpecialCase_Colon }, // :
   { ReportUnknownSymbol }, // ;
   { CheckEqualsDigraph }, // <
   { CheckEqualsDigraph }, // =
   { CheckEqualsDigraph }, // >
   { ConsumeIdentifier }, // ?
   { CheckSpacingAndConsumeUnigraphSymbol }, // @

   { ConsumeIdentifier },                    // A
   { ConsumeIdentifier },                    // B
   { ConsumeIdentifier },                    // C
   { ConsumeIdentifier },                    // D
   { ConsumeIdentifier },                    // E
   { ConsumeIdentifier },                    // F
   { ConsumeIdentifier },                    // G
   { ConsumeIdentifier },                    // H
   { ConsumeIdentifier },                    // I
   { ConsumeIdentifier },                    // J
   { ConsumeIdentifier },                    // K
   { ConsumeIdentifier },                    // L
   { ConsumeIdentifier },                    // M
   { ConsumeIdentifier },                    // N
   { ConsumeIdentifier },                    // O
   { ConsumeIdentifier },                    // P
   { ConsumeIdentifier },                    // Q
   { ConsumeIdentifier },                    // R
   { ConsumeIdentifier },                    // S
   { ConsumeIdentifier },                    // T
   { ConsumeIdentifier },                    // U
   { ConsumeIdentifier },                    // V
   { ConsumeIdentifier },                    // W
   { ConsumeIdentifier },                    // X
   { ConsumeIdentifier },                    // Y
   { ConsumeIdentifier },                    // Z

   { ConsumeUnigraphSymbol },                // [
   { ReportUnknownSymbol },                  // Backslash
   { ConsumeUnigraphSymbol },                // ]
   { ConsumeUnigraphSymbol },                // ^
   { ConsumeIdentifier },                    // _
   { ConsumeUnigraphSymbol },                // `

   { ConsumeIdentifier },                    // a
   { ConsumeIdentifier },                    // b
   { ConsumeIdentifier },                    // c
   { ConsumeIdentifier },                    // d
   { ConsumeIdentifier },                    // e
   { ConsumeIdentifier },                    // f
   { ConsumeIdentifier },                    // g
   { ConsumeIdentifier },                    // h
   { ConsumeIdentifier },                    // i
   { ConsumeIdentifier },                    // j
   { ConsumeIdentifier },                    // k
   { ConsumeIdentifier },                    // l
   { ConsumeIdentifier },                    // m
   { ConsumeIdentifier },                    // n
   { ConsumeIdentifier },                    // o
   { ConsumeIdentifier },                    // p
   { ConsumeIdentifier },                    // q
   { ConsumeIdentifier },                    // r
   { ConsumeIdentifier },                    // s
   { ConsumeIdentifier },                    // t
   { ConsumeIdentifier },                    // u
   { ConsumeIdentifier },                    // v
   { ConsumeIdentifier },                    // w
   { ConsumeIdentifier },                    // x
   { ConsumeIdentifier },                    // y
   { ConsumeIdentifier },                    // z

   { ConsumeUnigraphSymbol },                // {
   { ReportUnknownSymbol },                  // |
   { ConsumeUnigraphSymbol },                // }
   { ConsumeIdentifier },                    // ~
   { ReportUnknownSymbol }                   // DEL
};

// static void ReportInvalidSpacing(Lexer_Concrete_t *instance, CharacterClass_t class)

static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokenList)
{
   REINTERPRET(instance, interface, Lexer_Concrete_t *);
   instance->beginning = source;
   instance->tokenList = tokenList;
   // memset(&instance->token, 0, sizeof(Token_t));

   while((instance->current = Peek(instance)) != '\0')
   {
      actionTable[(*instance->current)].action(instance);
   }
}

void Lexer_Concrete_Init(Lexer_Concrete_t *instance, I_Error_t *errorHandler)
{
   instance->interface.lex = &lex;

   instance->errorHandler = errorHandler;
}
