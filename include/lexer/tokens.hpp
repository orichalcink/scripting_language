#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <cstdint>
#include <string>

enum class TType : std::int8_t
{
   macro, keyword, identifier, integer, real, string, character,
   equals, equals_equals, and_, or_, smaller, smaller_equal, bigger, bigger_equal, bang, bang_equal,
   macro_equals, macro_bang_equals,
   semicolon, dot_dot_dot, hash_hash, l_paren, r_paren, comma,
   newline, skip, eoi, eof
};

struct Token
{
   TType type;
   std::string lexeme;
};

#define con(value) case TType::value: return #value;

constexpr const char* token_to_string(TType type)
{
   switch (type)
   {
   con(macro)
   con(keyword)
   con(identifier)
   con(integer)
   con(real)
   con(string)
   con(character)
   con(equals)
   con(equals_equals)
   con(and_)
   con(or_)
   con(smaller)
   con(smaller_equal)
   con(bigger)
   con(bigger_equal)
   con(bang)
   con(bang_equal)
   con(macro_equals)
   con(macro_bang_equals)
   con(semicolon)
   con(dot_dot_dot)
   con(hash_hash)
   con(l_paren)
   con(r_paren)
   con(comma)
   con(newline)
   con(skip)
   con(eoi)
   con(eof)
   }
}

#undef mac

#endif // TOKENS_HPP
