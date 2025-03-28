#ifndef ERRORS_HPP
#define ERRORS_HPP

#define error constexpr const char*

namespace err
{
   // REPL errors
   error invalid_run_command = "Invalid run command, expected the second argument to be a valid file.";

   // Argument errors
   error out_of_bounds_arg = "Tried to access out of bounds argument.";
   error arg_redefined = "Argument appears more than once in the input.";
   error invalid_run_arg = "Invalid run argument value, expected an integer.";

   // File errors
   error invalid_input = "Invalid input.";
   error cannot_open_file = "Could not open the given file.";

   // Lexer errors
   error unterminated_comment = "Unterminated multi-line comment.";
   error unterminated_string = "Unterminated string.";
   error invalid_escape_code = "Invalid escape code in character/string.";
   error invalid_char = "Invalid character while lexing, characters can only be one character in size.";
   error unexpected_char = "Unexpected character while lexing.";
   error invalid_quotes = "Invalid single quote placement in number.";
   error invalid_real_number = "Invalid real number with multiple dots.";

   // Preprocessor errors
   error expected_ident_macro_def = "Expected an identifier after macro definition.";
   error expected_equals_macro_def = "Expected '=' after macro identifier in macro definition.";
   error macro_exists = "Tried to define a macro that already exists.";
   error invalid_macro_params = "Invalid macro parameters in macro definition, either use no parentheses or parentheses with parameters.";
   error invalid_macro_body = "Invalid macro body, expected atleast one token. Use ';' instead of '=' if pure definition macro is needed.";
   error invalid_mcond_start = "Macro conditionals must start with '#if'/'#ifn'/'#ifdef'/'#ifndef'.";
   error expected_comma_or_r_paren = "Expected a ',' or a ')' after a parameter in macro definition.";
   error invalid_macro_call = "Invalid macro call, either unclosed parentheses or parentheses without arguments.";
   error called_empty_macro = "Tried to call a macro that was defined without a body.";
   error invalid_arg_count = "Tried to call a macro where the argument count did not match the definition parameter count.";
   error infinite_macro_loop = "Detected infinite macro loop, if this was a mistake, set '--macro-dept' run argument to a higher value.";
   error statement_semicolon = "Expected statement/macro to end in a semicolon.";
   error invalid_concatenation_macro = "Invalid concatenation macro, expected two operands.";
   error invalid_equality_macro = "Invalid equality/inequality macro, expected two operands.";
   error invalid_undefine = "Expected a macro identifier after the '#undefine'/'#undef' keyword.";
   error invalid_variadic_macro = "Invalid variadic macro, the '...' operator can only be used once and only at the end of the parameter list.";
   error expected_string_after_error = "Expected a string after the error macro.";
   error import_invalid_file = "Tried to import a file that does not exist.";
   error expected_file = "Expected a file after '#include'/'#import' keyword.";
} // namespace err

#undef error

#endif // ERRORS_HPP
