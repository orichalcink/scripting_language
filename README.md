# Scripting language
Scripting language by chalcinxx. Version 1.0.1.

## How it works
1. The input is retrieved from the user and parsed into arguments.
2. The program chooses what to do based on the arguments.
3. If `run` argument is present, then open the file and use that as the input, else use the whole input.
4. The input is turned into tokens by the lexer.
5. The preprocessor handles macros, imports and macro conditionals.

## Features:
- [Macros](#macros)
- - [Importing](#importing)
- - [Defines](#defines)
- - [Variadic macros](#variadic-macros)
- - [Macro string operations](#macro-string-operations)
- - [Macro equality operations](#macro-equality-operations)
- - [Un-defines](#un-defines)
- - [Errors](#errors)
- - [Logging](#logging)
- - [Predefined macros](#predefined-macros)
- [Misc](#misc)
- - [Comments](#comments)
- - [Newline operator](#newline-operator)
- [Run arguments](#run-args)
- - [Executing a file](#executing-a-file)
- - [Executing code on the fly](#execute-code-on-the-fly)
- - [Run arguments](#run-arguments)

# Macros
### Importing
Other files can be imported using this syntax:
```c
#import "file.ext";
#include "file.ext";

#import "file1.ext", "file2.ext";
#include "file1.ext", "file2.ext";
```
Here's an example:
> include.q
```c
mut let x = 20;
```
> script.q
```c
#import "include.q";
mut let y = 30;
```
After processing:
```c
mut let x = 20;
mut let y = 30;
```
The difference between `#import` and `#include` is that `#import` checks if the file has been imported already and will automatically prevent importing the same file multiple times, but `#include` does not and allows to include the same file as many times as is needed. Here's an example:
> include.q
```c
mut let x = 10;
```
> script1.q
```c
#import "include.q", "include.q", "include.q";
```
After processing `script1.q`:
```c
mut let x = 10;
```
> script2.q
```c
#include "include.q", "include.q", "include.q";
```
After processing `script2.q`:
```c
mut let x = 10;
mut let x = 10;
mut let x = 10;
```
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
#def stringify(...) = "...";
stringify(a, b, c, d, e, f) // Replaces with: "a b c d e f"
```
Parameters can also be defined before the `...` operator, as long as the `...` operator is last and is not used more than once:
```c
#def fun(a, b, c, ...) = a AND b AND c OR ...;
fun(let, mut, thing, lala, 'c', "string") // Replaces with: let AND mut AND thing OR lala 'c' "string"
```
### Macro string operations
Macro arguments can be stringified by putting them in between double quotes:
```c
#def stringify2(x, y) = "x", "y";
stringify2(let, mut) // Replaces with: "let", "mut"
```
This only works in parametrized macros for the parameters, and only if the string contents match the parameter identifier.

Strings can also be concatenated during processing using the `##` operator like this:
```c
#def stringify2(x, y) = "x" " " "y" ## ##;
stringify2(mut let x, = 20;) // Replaces with: "mut let x = 20 ;" 
```
This might look confusing, because the `##` operator concatenates two tokens that are before it, so in this case, the first `##` operator concatenates `"y"` and `" "` together and the second one concatenates `"x"` and `" y"` together.

Note that the `##` operator does not automatically separate the arguments with a space, but `"` do.
### Macro equality operations
Two token lexemes can be checked at the preprocessor stage with `#=` and `#!` operators:
```c
let "let" #= // true, because they have the same lexeme
10 "10" #! // false, because they have the same lexeme
```
Just like the `##` operator, these two operators take the previous two tokens and compare them.
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
Nothing will happen if you try to undefine a macro that is not defined.
### Errors:
Errors can be thrown using the `#error` keyword, the errors will be thrown using the same system that any syntax, file and other errors are thrown with:
```c
#error "Error: something went wrong!";
```
### Logging:
Anything at the preprocessor-level can be logged with the `#log` keyword using this syntax:
```c
#log BODY;
#logl BODY
```
The difference between `#log` and `#logl` is that `#log` will log everything until the first `;` that isn't used by another statement (more on that later), but `#logl` will log everything until the first new line.

Here are some examples:
```c
#log 10; // 10
#log "Hello, World!"; // Hello, World!
#log mut let x = 20; // mutletx=20 

#def x = 20 && 40;
#log x; // 20&&40
```
Statements can be placed inside of the log statement and they'll work as usual:
```c
#log #def x = 20; 20; // 20
#log x; // 20, x is still defined

#log #def y(...) = "..."; y(1, 2, 3, 4, 5); // 1 2 3 4 5
#log y(mut, let) // mut let, y is still defined
```
We can also create a prettier version of `#log` that uses a space as a separator using the variadic macros:
```c
#def pretty_log(...) = #logl "..." ;; ;
pretty_log(12, 3, 4, 5, "string") // 12 3 4 5 string
```
### Predefined macros
Predefined macros can be used like any other macro, even undefined if needed:
```c
#log __DATETIME__; // Logs the current date and time
```
Here's the list of all of them:
- `__FILE__` - Current file.
- `__VERSION__` - Current version as integer.
- `__VERSION_MAJOR__` - Major version.
- `__VERSION_MINOR__` - Minor version.
- `__VERSION_PATCH__` - Patch version.
- `__VERSION_STR__` - Version string.
- `__EPOCH__` - Time in seconds since epoch.
- `__EPOCH_NS__` - Time in nanoseconds since epoch.
- `__DATE__` - The current date.
- `__TIME__` - The current time.
- `__DATETIME__` - The current date and time.
- `__OS__` - Name of the OS ("Windows", "Linux" or "MacOS").

List of empty macros purely for macro conditionals:
- `__WIN__` - User is using Windows.
- `__LINUX__` - User is using Linux.
- `__MACOS__` - User is using MacOS.
- `__64BIT__` - User is running a 64bit machine.
- `__32BIT__` - User is running a 32bit machine.
# Misc
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

# Run args
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
- `--macro-depth=INTEGER` - Set the maximum macro recursion that is used for preventing infinite macro loops.
- `--no-predefined-macros` - Do not define any predefined macros.
