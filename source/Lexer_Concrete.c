/***
 * File: Lexer_Concrete.c
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "Lexer_Concrete.h"
#include "util.h"

enum
{
   Touchy_DontCare,
   Touchy_Yes,
   Touchy_No
};
typedef uint8_t Rule_Touchy_t;

typedef struct
{
   void (*action)(Lexer_Concrete_t *instance);
   Token_Type_t type;
   Token_Type_t secondaryType;
   Rule_Touchy_t touch;
} Rule_Entry_t;

/*********************************
 * Movement through source string
 *********************************/
static inline char Peek(Lexer_Concrete_t *instance)
{
   return *instance->current;
}

static inline char PeekNext(Lexer_Concrete_t *instance)
{
   return (instance->current[1] == '\0') ? ' ' : instance->current[1];
}

static inline char PeekPrevious(Lexer_Concrete_t *instance)
{
   return (instance->current == instance->beginning) ? ' ' : *(instance->current - 1);
}

static inline char PeekAhead(Lexer_Concrete_t *instance, size_t ahead)
{
   return (instance->current[ahead] == '\0') ? ' ' : instance->current[ahead];
}

static inline void AdvanceOne(Lexer_Concrete_t *instance)
{
   instance->current++;
}

static inline void AdvanceMany(Lexer_Concrete_t *instance, size_t many)
{
   instance->current += many;
}

static void AddToken(Lexer_Concrete_t *instance, Token_Type_t type, const char *lexeme, size_t length, size_t line)
{
   instance->token.type = type;
   instance->token.lexeme = lexeme;
   instance->token.length = length;
   instance->token.line = line;

   List_Add(instance->tokenList, &instance->token);
}

// Forward declaration because circular access ruleTable <--> functions
static void Ignore(Lexer_Concrete_t *instance);
static void IncrementLineCounter(Lexer_Concrete_t *instance);
static void ReportUnexpectedCharacter(Lexer_Concrete_t *instance);
static void HandleSpecialCase_SingleQuote(Lexer_Concrete_t *instance);
static void ConsumeIdentifier(Lexer_Concrete_t *instance);
static void CheckSpacing(Lexer_Concrete_t *instance, uint8_t length);
static void ConsumeWideSymbol(Lexer_Concrete_t *instance, uint8_t length, Token_Type_t type);
static void ConsumeSymbol(Lexer_Concrete_t *instance);
static void CheckDigraphWithEqualSignThenConsumeSymbol(Lexer_Concrete_t *instance);
static void ConsumeNumberLiteralOrSpecialIdentifier(Lexer_Concrete_t *instance);
static void ConsumeStringLiteral(Lexer_Concrete_t *instance);
static void CheckDigraphWithEqualSignThenConsumeSymbol(Lexer_Concrete_t *instance);
static void SpecialCase_Exclamation(Lexer_Concrete_t *instance);
static void SpecialCase_Pound(Lexer_Concrete_t *instance);
static void SpecialCase_Dot(Lexer_Concrete_t *instance);
static void SpecialCase_Colon(Lexer_Concrete_t *instance);
static void SpecialCase_Dash(Lexer_Concrete_t *instance);
static void SpecialCase_Tilde(Lexer_Concrete_t *instance);


#define DEFAULT_RULE_ENTRY_UNEXPECTED .action = ReportUnexpectedCharacter
#define DEFAULT_RULE_ENTRY_SPACE .action = Ignore

static const Rule_Entry_t ruleTable[128] =
{
   { .action = ReportUnexpectedCharacter }, // null
   { .action = ReportUnexpectedCharacter }, // start of heading
   { .action = ReportUnexpectedCharacter }, // start of text
   { .action = ReportUnexpectedCharacter }, // end of text
   { .action = ReportUnexpectedCharacter }, // end of transmission
   { .action = ReportUnexpectedCharacter }, // enquiry
   { .action = ReportUnexpectedCharacter }, // ACK
   { .action = ReportUnexpectedCharacter }, // bell
   { .action = ReportUnexpectedCharacter }, // backspace
   { .action = Ignore },                    // horizontal tab
   { .action = IncrementLineCounter },      // LF
   { .action = Ignore },                    // vertical tab
   { .action = Ignore },                    // FF
   { .action = Ignore },                    // CR
   { .action = ReportUnexpectedCharacter }, // shift out
   { .action = ReportUnexpectedCharacter }, // shift in
   { .action = ReportUnexpectedCharacter }, // data link escape
   { .action = ReportUnexpectedCharacter }, // device control 1
   { .action = ReportUnexpectedCharacter }, // device control 2
   { .action = ReportUnexpectedCharacter }, // device control 3
   { .action = ReportUnexpectedCharacter }, // device control 4
   { .action = ReportUnexpectedCharacter }, // NAK
   { .action = ReportUnexpectedCharacter }, // SYN
   { .action = ReportUnexpectedCharacter }, // end of transmission block
   { .action = ReportUnexpectedCharacter }, // cancel
   { .action = ReportUnexpectedCharacter }, // end of medium
   { .action = ReportUnexpectedCharacter }, // substitute
   { .action = ReportUnexpectedCharacter }, // escape
   { .action = ReportUnexpectedCharacter }, // file separator
   { .action = ReportUnexpectedCharacter }, // group separator
   { .action = ReportUnexpectedCharacter }, // record separator
   { .action = ReportUnexpectedCharacter }, // unit separator

   { .action = Ignore }, // Space
   { .action = SpecialCase_Exclamation, .secondaryType = Token_Type_BangEqual, .touch = Touchy_Yes }, // !
   { .action = ConsumeStringLiteral }, // "
   { .action = SpecialCase_Pound, .type = Token_Type_Pound, .touch = Touchy_Yes }, // #
   { .action = ConsumeSymbol, .type = Token_Type_Dollar, .touch = Touchy_Yes }, // $
   { .action = ReportUnexpectedCharacter }, // %
   { .action = ReportUnexpectedCharacter }, // &
   { .action = HandleSpecialCase_SingleQuote }, // '
   { .action = ConsumeSymbol, .type = Token_Type_Paren_Left }, // (
   { .action = ConsumeSymbol, .type = Token_Type_Paren_Right }, // )
   { .action = ConsumeSymbol, .type = Token_Type_Asterisk, .touch = Touchy_Yes }, // *
   { .action = ConsumeSymbol, .type = Token_Type_Plus, .touch = Touchy_Yes }, // +
   { .action = ConsumeSymbol, .type = Token_Type_Comma }, // ,
   { .action = SpecialCase_Dash, .type = Token_Type_Dash, .touch = Touchy_Yes }, // -
   { .action = SpecialCase_Dot }, // .
   { .action = ConsumeSymbol, .type = Token_Type_Slash, .touch = Touchy_Yes }, // /

   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 0
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 1
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 2
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 3
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 4
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 5
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 6
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 7
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 8
   { .action = ConsumeNumberLiteralOrSpecialIdentifier }, // 9

   { .action = SpecialCase_Colon }, // :
   { .action = ReportUnexpectedCharacter }, // ;
   { .action = CheckDigraphWithEqualSignThenConsumeSymbol, .type = Token_Type_AngleBracket_Left, .secondaryType = Token_Type_LessEqual, .touch = Touchy_Yes }, // <
   { .action = CheckDigraphWithEqualSignThenConsumeSymbol, .type = Token_Type_Equal, .secondaryType = Token_Type_EqualEqual, .touch = Touchy_Yes }, // =
   { .action = CheckDigraphWithEqualSignThenConsumeSymbol, .type = Token_Type_AngleBracket_Right, .secondaryType = Token_Type_GreaterEqual, .touch = Touchy_Yes }, // >
   { .action = ConsumeIdentifier }, // ?
   { .action = ConsumeSymbol, .type = Token_Type_Arroba, .touch = Touchy_Yes }, // @

   { .action = ConsumeIdentifier },                    // A
   { .action = ConsumeIdentifier },                    // B
   { .action = ConsumeIdentifier },                    // C
   { .action = ConsumeIdentifier },                    // D
   { .action = ConsumeIdentifier },                    // E
   { .action = ConsumeIdentifier },                    // F
   { .action = ConsumeIdentifier },                    // G
   { .action = ConsumeIdentifier },                    // H
   { .action = ConsumeIdentifier },                    // I
   { .action = ConsumeIdentifier },                    // J
   { .action = ConsumeIdentifier },                    // K
   { .action = ConsumeIdentifier },                    // L
   { .action = ConsumeIdentifier },                    // M
   { .action = ConsumeIdentifier },                    // N
   { .action = ConsumeIdentifier },                    // O
   { .action = ConsumeIdentifier },                    // P
   { .action = ConsumeIdentifier },                    // Q
   { .action = ConsumeIdentifier },                    // R
   { .action = ConsumeIdentifier },                    // S
   { .action = ConsumeIdentifier },                    // T
   { .action = ConsumeIdentifier },                    // U
   { .action = ConsumeIdentifier },                    // V
   { .action = ConsumeIdentifier },                    // W
   { .action = ConsumeIdentifier },                    // X
   { .action = ConsumeIdentifier },                    // Y
   { .action = ConsumeIdentifier },                    // Z

   { .action = ConsumeSymbol, .type = Token_Type_SquareBrace_Left },                // [
   { .action = ReportUnexpectedCharacter },                  // backslash
   { .action = ConsumeSymbol, .type = Token_Type_SquareBrace_Right },                // ]
   { .action = ReportUnexpectedCharacter },                // ^
   { .action = ConsumeIdentifier },                    // _
   { .action = ConsumeSymbol, .type = Token_Type_Backtick },                // `

   { .action = ConsumeIdentifier },                    // a
   { .action = ConsumeIdentifier },                    // b
   { .action = ConsumeIdentifier },                    // c
   { .action = ConsumeIdentifier },                    // d
   { .action = ConsumeIdentifier },                    // e
   { .action = ConsumeIdentifier },                    // f
   { .action = ConsumeIdentifier },                    // g
   { .action = ConsumeIdentifier },                    // h
   { .action = ConsumeIdentifier },                    // i
   { .action = ConsumeIdentifier },                    // j
   { .action = ConsumeIdentifier },                    // k
   { .action = ConsumeIdentifier },                    // l
   { .action = ConsumeIdentifier },                    // m
   { .action = ConsumeIdentifier },                    // n
   { .action = ConsumeIdentifier },                    // o
   { .action = ConsumeIdentifier },                    // p
   { .action = ConsumeIdentifier },                    // q
   { .action = ConsumeIdentifier },                    // r
   { .action = ConsumeIdentifier },                    // s
   { .action = ConsumeIdentifier },                    // t
   { .action = ConsumeIdentifier },                    // u
   { .action = ConsumeIdentifier },                    // v
   { .action = ConsumeIdentifier },                    // w
   { .action = ConsumeIdentifier },                    // x
   { .action = ConsumeIdentifier },                    // y
   { .action = ConsumeIdentifier },                    // z

   { .action = ConsumeSymbol, .type = Token_Type_CurlyBrace_Left },                // {
   { .action = ReportUnexpectedCharacter },                  // |
   { .action = ConsumeSymbol, .type = Token_Type_CurlyBrace_Right },                // }
   { .action = ConsumeIdentifier },                    // ~
   { .action = ReportUnexpectedCharacter }                   // DEL
};

// static void ReportInvalidSpacing(Lexer_Concrete_t *instance, CharacterClass_t class)

static void lex(I_Lexer_t *interface, const char *source, I_List_t *tokenList)
{
   REINTERPRET(instance, interface, Lexer_Concrete_t *);
   instance->beginning = source;
   instance->current = source;
   instance->tokenList = tokenList;
   instance->line = 1;

   while(Peek(instance) != '\0')
   {
      ruleTable[Peek(instance)].action(instance);
   }
}

void Lexer_Concrete_Init(Lexer_Concrete_t *instance, I_Error_t *errorHandler)
{
   instance->interface.lex = &lex;
   instance->errorHandler = errorHandler;
}

/*********************************
 * Actions
 *********************************/
static void Ignore(Lexer_Concrete_t *instance)
{
   AdvanceOne(instance);
}

static void IncrementLineCounter(Lexer_Concrete_t *instance)
{
   instance->line++;
}

static void ReportUnexpectedCharacter(Lexer_Concrete_t *instance)
{
   if(iscntrl(Peek(instance)))
   {
      Error_Report(instance->errorHandler, "Unexpected non-printable character");
   }
   else
   {
      char message[25] = "Unexpected character ' '";
      message[22] = Peek(instance);
      Error_Report(instance->errorHandler, message);
   }

   AdvanceOne(instance);
}

static void HandleSpecialCase_SingleQuote(Lexer_Concrete_t *instance)
{

}

static void ConsumeIdentifier(Lexer_Concrete_t *instance)
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
      Error_Report(instance->errorHandler, "Bad identifier name.");
   }
}

static void CheckSpacing(Lexer_Concrete_t *instance, uint8_t length)
{
   char message[66];
   bool touchyOnLeft = ruleTable[PeekPrevious(instance)].touch == Touchy_Yes;
   bool touchyOnRight = ruleTable[PeekAhead(instance, length)].touch == Touchy_Yes;

   if(touchyOnLeft && touchyOnRight)
   {
      sprintf(message, "\"Touchy\" symbol '%.*s' next to other touchy symbols '%c' and '%c'",  length, instance->current, PeekPrevious(instance), PeekAhead(instance, length));
      Error_Report(instance->errorHandler, message);
   }
   else if(touchyOnLeft)
   {
      sprintf(message, "\"Touchy\" symbol '%.*s' next to another touchy symbol '%c'",  length, instance->current, PeekPrevious(instance));
      Error_Report(instance->errorHandler, message);
   }
   else if(touchyOnRight)
   {
      sprintf(message, "\"Touchy\" symbol '%.*s' next to another touchy symbol '%c'",  length, instance->current, PeekAhead(instance, length));
      Error_Report(instance->errorHandler, message);
   }
}

static void ConsumeWideSymbol(Lexer_Concrete_t *instance, uint8_t length, Token_Type_t type)
{
   if(ruleTable[Peek(instance)].touch == Touchy_Yes)
   {
      CheckSpacing(instance, length);
   }

   AddToken(instance, type, instance->current, length, instance->line);
   AdvanceMany(instance, length);
}

static void ConsumeSymbol(Lexer_Concrete_t *instance)
{
   ConsumeWideSymbol(instance, 1, ruleTable[Peek(instance)].type);
}

static void CheckDigraphWithEqualSignThenConsumeSymbol(Lexer_Concrete_t *instance)
{
   if(PeekNext(instance) == '=')
   {
      ConsumeWideSymbol(instance, 2, ruleTable[Peek(instance)].secondaryType);
   }
   else
   {
      ConsumeWideSymbol(instance, 1, ruleTable[Peek(instance)].type);
   }
}

static void ConsumeNumberLiteralOrSpecialIdentifier(Lexer_Concrete_t *instance)
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

static void ConsumeStringLiteral(Lexer_Concrete_t *instance)
{
   const char *beginning = instance->current;
   size_t line = instance->line;
   size_t length = 1;

   AdvanceOne(instance);   // Past opening "
   while(Peek(instance) != '"')
   {
      if(Peek(instance) == '\0')
      {
         Error_Report(instance->errorHandler, "String literal missing ending \"");
         return;
      }
      if(Peek(instance) == '\n')
      {
         Error_Report(instance->errorHandler, "String literal not contained on one line.");
         instance->line++;
         // Do not break, so it will keep consuming on the next line to the end of the string
      }

      AdvanceOne(instance);
      length++;
   }

   AdvanceOne(instance);   // Past closing "
   length++;

   AddToken(instance, Token_Type_Literal_String, beginning, length, instance->line);
}

static void SpecialCase_Exclamation(Lexer_Concrete_t *instance)
{
   if(PeekNext(instance) == '=')
   {
      ConsumeWideSymbol(instance, 2, ruleTable[Peek(instance)].secondaryType);
   }
   else
   {
      ConsumeIdentifier(instance);
   }
}

static void CheckNumberOrIdentifier(Lexer_Concrete_t *instance)
{

}

static void SpecialCase_Pound(Lexer_Concrete_t *instance)
{
   if(isalpha(PeekNext(instance))
      || PeekNext(instance) == '_'
      || PeekNext(instance) == '-'
      || PeekNext(instance) == '#'
      || PeekNext(instance) == '!'
      || PeekNext(instance) == '?')
   {
      ConsumeIdentifier(instance);
   }
   else
   {
      ConsumeWideSymbol(instance, 1, Token_Type_Pound);
   }
}

static void SpecialCase_Dot(Lexer_Concrete_t *instance)
{
   ConsumeWideSymbol(instance, 1, Token_Type_Dot);
}

static void SpecialCase_Colon(Lexer_Concrete_t *instance)
{

}

static void SpecialCase_Dash(Lexer_Concrete_t *instance)
{
   ConsumeWideSymbol(instance, 1, Token_Type_Dash);
}

static void SpecialCase_Tilde(Lexer_Concrete_t *instance)
{

}
