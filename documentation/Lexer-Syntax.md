# Syntax rules (for the Lexer)

Without going into too much detail about the language's grammar, this document will outline all of the syntax rules that the Lexer will follow to convert source code into meaningful tokens that can then be fed to the grammar rules of the parser.

# Identifiers and Keywords

**Identifiers** are all the "word-like" tokens in the language, including variables, functions, procedures, and type names. Meanwhile, **keywords** are reserved command words that have a special semantic meaning in the language.

With a few exception (see [Special Types](#special-types)), all valid identifiers follow the form `[\w_-#!?]*[\w?][\w_#-!?]*`, where `\w` are the word characters `[a-zA-Z]`. In other words, they must contain at least one word character, and any number of non-word characters `[_-#!?]`

Syntactically, keywords are a subset of identifiers, formed by a series of word characters (`[\w]+`). During the lexing step, no distinction needs to be made between identifiers and keywords and the binding of these tokens will be handled later by the parser. So they will not be listed here.

There are also a few built-in functions whose names are valid identifiers (e.g. `and`, `or`, and `not`). As with keywords, these will not be distinguished until the parsing step and will not be listed here.

# Literals

Literals are constructs which take on a value. In this language, there are three types: Strings, Numbers, and Symbols.

## String Literals

Strings literals are simply ascii text enclosed between double-quotes on a single line: `"[^"]*"`

```
"This is a valid string :)"
"This is
         not a valid string :("
```

(Note: Single-quoted strings, Multi-line strings, and Unicode may later be built into the language.)

## Number Literals

Numbers are numbers. A single decimal point might show up at the start, middle, or end. Syntactically, that's all there is to it: `[0-9]*[.]?[0-9]*`

```
1
234
.5
21746193741239461329847163601503086535018237563285761
21746193741239461329847163601503086535018237563285761.
2.1746193741239461329847163601503086535018237563285761
```

## Symbol Literals

Symbols are named constants with some underlying global value. They are useful for giving meaningful names to flags, statuses, and enumerations. They are represented with a colon, followed by any valid identifier `:[\w_-#!?]*[\w][\w_#-!?]*`

```
:thisIsAValidSymbol
:so-is-this
```

# "Symbolic" Tokens

All other tokens are made of one or more symbols and can have multiple uses in the language. Some of these symbols can appear directly adjacent to any other kind of token, including identifiers or literals. Meanwhile, many symbols are not allowed to appear next to an identifier or literal because 1) they cause ambiguity with identifier names or 2) enforcing consistent spacing around similar symbols will hopefully improve readability. Thus, these characters must either be surrounded by whitespace or other symbols that  The table below gives each of the symbolic tokens, their spacing rules, and whether they overlap with a valid identifier character.

Symbol | Can appear next to any token? | Can appear in an identifier? | Notes
:-----:|:-----------------------------:|:----------------------------:|:----:
`(`    | ✓                             |                              |
`)`    | ✓                             |                              |
`[`    | ✓                             |                              |
`]`    | ✓                             |                              |
`{`    | ✓                             |                              |
`}`    | ✓                             |                              |
`,`    | ✓                             |                              |
\`     | ✓                             |                              |
`.`    | ✓                             |                              |
`:`    | ✓                             |                              |
`?`    |                               | ✓                            | Any length sequence of question marks is a single token
`@`    |                               |                              |
`#`    |                               | ✓                            |
`$`    |                               |                              |
`-`    |                               | ✓                            |
`+`    |                               |                              |
`/`    |                               |                              |
`*`    |                               |                              |
`=`    |                               |                              |
`<`    |                               |                              |
`>`    |                               |                              |
`..`   |                               |                              |
`...`  |                               |                              |
`<=`   |                               |                              |
`>=`   |                               |                              |
`==`   |                               |                              |
`!=`   |                               |                              |

# Comments

For now, there will be no comments in this language. Totally not because I don't feel like implementing them right now, but because this language is naturally self-documenting ;)

# Special Types

There are a few special types built into the language that do not conform to the rules for identifier names.

Type         | Meaning                          | Example
:-----------:|:--------------------------------:|:--------------
`[?]+`       | Generic for parameterized types  | ??
`[0-9]*'`    | Numeric, N bytes                 | `x: 6'` => 6-byte number
`[0-9]*"`    | Numeric, N bits                  | `x: 3"` => 3-bit number
`~`          | Built-in linked list type        | `int[~]`
`-`          | Built-in sequential array type   | `int[-]`
`=`          | Built-in TBD type                | `int[=]`
`\[[0-9]+\]` | Built-in fixed-size array        | `int[20]`
`\[\]`       | Built-in abstract container type | `int[]`

# Unused characters

The following standard keyboard characters are currently unused:

```
%^&\|;
```

But uses will be found for them soon!
