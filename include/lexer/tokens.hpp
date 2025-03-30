#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <cstdint>
#include <string>

enum class TType : std::int8_t
{
   macro, keyword, identifier, integer, real, string, character,
   question, colon,
   equals, plus_equals, minus_equals, star_equals, slash_equals, percent_equals, shift_left_equals, shift_right_equals, bitwise_and_equals, bitwise_xor_equals, bitwise_or_equals, star_star_equals,
   logical_or, logical_and, bitwise_or, bitwise_xor, bitwise_and, 
   equals_equals, not_equals, smaller, smaller_equals, bigger, bigger_equals,
   shift_left, shift_right,
   plus, minus, star, slash, percent, star_star,
   logical_not, bitwise_not, plus_plus, right_plus_plus, minus_minus, right_minus_minus,
   dot, comma, dot_dot_dot, semicolon,
   hash_hash, hash_equals, hash_not_equals,
   l_paren, r_paren, l_bracket, r_bracket, l_brace, r_brace,
   newline, skip, eoi, eof
};

struct Token
{
   TType type;
   std::string lexeme;

   Token(TType type, const std::string& lexeme)
      : type(type), lexeme(lexeme) {}
};

#define c(value) case TType::value: return #value;

constexpr const char* token_to_string(TType type)
{
   switch (type)
   {
   c(macro) c(keyword) c(identifier) c(integer) c(real) c(string) c(character)
   c(question) c(colon)
   c(equals) c(plus_equals) c(minus_equals) c(star_equals) c(slash_equals) c(percent_equals) c(shift_left_equals) c(shift_right_equals) c(bitwise_and_equals) c(bitwise_xor_equals) c(bitwise_or_equals) c(star_star_equals)
   c(logical_or) c(logical_and) c(bitwise_or) c(bitwise_xor) c(bitwise_and)
   c(equals_equals) c(not_equals) c(smaller) c(smaller_equals) c(bigger) c(bigger_equals)
   c(shift_left) c(shift_right)
   c(plus) c(minus) c(star) c(slash) c(percent) c(star_star)
   c(logical_not) c(bitwise_not) c(plus_plus) c(right_plus_plus) c(minus_minus) c(right_minus_minus)
   c(dot) c(comma) c(dot_dot_dot) c(semicolon)
   c(hash_hash) c(hash_equals) c(hash_not_equals)
   c(l_paren) c(r_paren) c(l_bracket) c(r_bracket) c(l_brace) c(r_brace)
   c(newline) c(skip) c(eoi) c(eof)
   }
}

#undef c

#endif // TOKENS_HPP
