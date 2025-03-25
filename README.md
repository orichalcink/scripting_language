# Scripting language
Scripting language by chalcinxx.

## How it works
1. The input is retrieved from the user and parsed into arguments.
2. The program chooses what to do based on the arguments.
3. If `run` argument is present, then open the file and use that as the input, else use the whole input.
4. The input is turned into tokens by the lexer.
5. The preprocessor handles macros, imports and macro conditionals.

## Features:
- [Macros](#macros)
- - [Defines](#defines)
- - [Variadic macros](#variadic-macros)
- - [Macro string operations](#macro-string-operations)
- - [Un-defines](#un-defines)
- [Misc](#misc)
- - [Comments](#comments)
- - [Newline operator](#newline-operator)
- [Run arguments](#run-args)
- - [Executing a file](#executing-a-file)
- - [Executing code on the fly](#execute-code-on-the-fly)
- - [Run arguments](#run-arguments)

## Macros
### Defines
Macros can be defined with this syntax:
```c
#def IDENTIFIER = MACRO_BODY;
#defl IDENTIFIER = MACRO_BODY

IDENTIFIER // Macro call

#def IDENTIFIER(PARAM1, PARAM2) = MACRO_BODY;
#defl IDENTIFIER(PARAM1, PARAM2) = MACRO_BODY

IDENTIFIER(ARG1, ARG2) // Macro call
```
The difference between `#def` and `#defl` is that `#def` statement ends at the first encountered `;`, but `#defl` ends at the end of the line or at the `;;` operator.

Here's an example:
```c
#def x = 10;
#def y = 20;
#defl z = mut let var1 = x; mut let var2 = y;
z // Replaces with: mut let var1 == 10; mut let var2 = 20;

#def stringify(x) = "x";
stringify(something) // Replaces with: "something"
```
### Variadic macros
Variadic macros can be defined using the `...` operator with this syntax:
```c
#def IDENTIFIER(...) = MACRO_BODY;
#defl IDENTIFIER(...) = MACRO_BODY
```
Here's an example:
```c
#def stringify(...) = ...;
stringify(a, b, c, d, e, f) // Replaces with: "a b c d e f"
```
The parameters get turned into one giant string and then any `...` or `"..."` found get replaced by it.
### Macro string operations
Macro arguments can be stringified by putting them in between double quotes:
```c
#def stringify2(x, y) = "x", "y";
stringify2(let, mut) // Replaces with: "let", "mut"
```
This only works in parametrized macros for the parameters, and only if the string contents match the parameter identifier.

Strings can also be concatenated during processing using the `##` operator like this:
```c
#def stringify2(x, y) = "x" ## " " ## "y";
stringify2(mut let x, = 20;) // Replaces with: "mut let x = 20 ;" 
```
Note that the `##` operator does not separate the arguments with a space, but `"` do.
### Macro equality operations
Two token lexemes can be checked at the preprocessor stage with `#=` and `#!` operators:
```c
let #= "let" // true, because they have the same lexeme
10 #! "10" // false, because they have the same lexeme
```
### Un-defines:
Macros can be undefined using the following syntax:
```c
#undef IDENTIFIER;
```
Here's an example:
```c
x // Identifier "x"
#def x = 20;
x // Integer 20
#undef x;
x // Identifier "x"
```
Nothing will happen if you try to un-define a macro that is not defined.
## Misc
### Comments:
Comments are the same as C-style comments:
```c
// This is a single line comment.
/*
This is a
multi line
comment
*/
mut let var; // My variable
```
### Newline operator:
Newlines can be inserted with the `;;` operator:
```c
mut let x = 10; ;; mut let y = 20; ;; mut let z = 30;
```
This will expand to:
```c
mut let x = 10;
mut let y = 20;
mut let z = 30;
```

## Run args
### Executing a file
To execute a file, the `run` argument must be used and the file after:
```
> run scripts/script.q
```
Arguments can be added after that if needed:
```
> run scripts/script.q --log-lexer --log-preprocessor
```
Unknown arguments `--invalid-run-arg=100` or misspelt arguments `--log-preprocesor` will be ignored, unless they have an invalid value that is not an integer.
### Execute code on the fly
To execute code, just type it in the REPL, as long as it does not start with `run`, `help` or `quit`:
```
> #def x = 10; #def y = 20; x && y;
```
The downside to this is that arguments cannot be used. Remember that newlines can be inserted using the `;;` operator if needed.
### Run arguments
Run arguments are arguments that go after the file in the `run` command:
```
> run scripts/script.q --log-lexer --log-preprocessor
```
Here's the list of all of them:
- `--log-lexer` - Display all tokens after lexing the input.
- `--log-preprocessor` - Display all tokens after processing the tokens.
- `--skip-preprocessor` - Skip processing the tokens in the preprocessor.
- `--bench` - Measure and display the execution time.
- `--macro-depth=INTEGER` - set the maximum macro recursion that is used for preventing infinite macro loops.
