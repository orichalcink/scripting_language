#ifndef ERRORS_HPP
#define ERRORS_HPP

#define error constexpr const char*

namespace err
{
   // REPL errors
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
} // namespace err

#undef error

#endif // ERRORS_HPP
