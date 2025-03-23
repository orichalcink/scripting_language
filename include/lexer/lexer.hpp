#ifndef LEXER_H
#define LEXER_H

#include "errors/catcher.hpp"
#include "lexer/tokens.hpp"

class Lexer
{
public:
   Lexer(Catcher& catcher, std::string& string);
   ~Lexer() = default;

   std::vector<Token>& tokenize();

   void push_token(TType type, const std::string& lexeme);
   void push_token_ad(TType type, const std::string& lexeme);
   
   char advance();
   char peek() const;
   char prev() const;

private:
   Catcher& catcher;
   std::string& source;
   std::vector<Token> tokens;
   size_t index = 0;
};

#endif // LEXER_H
