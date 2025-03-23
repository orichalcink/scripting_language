#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <cstdint>
#include <string>

enum class TType : std::int8_t
{
   macro, keyword, identifier, integer, real, string, character,
   equals, equals_equals, and_, or_, smaller, smaller_equal, bigger, bigger_equal, bang, bang_equal,
   semicolon, l_paren, r_paren, comma,
   newline, skip, eof
};

struct Token
{
   TType type;
   std::string lexeme;
};

#endif // TOKENS_HPP
