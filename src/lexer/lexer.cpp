#include "lexer/lexer.hpp"
#include "errors/errors.hpp"
#include "lexer/keywords.hpp"

using namespace std::string_literals;

Lexer::Lexer(Catcher& catcher, std::string& source)
   : catcher(catcher), source(source), size(source.size()) {}

std::vector<Token>& Lexer::tokenize()
{
   for (; this->index < this->size; ++this->index)
   {
      char ch = this->source.at(this->index);

      if (ch == '\n')
         push_token(TType::newline, "\n"s);
      else if (isspace(ch))
         continue;
      else if (ch == '/' && peek() == '/')
         for (; this->index < this->size && this->source.at(this->index) != '\n'; ++this->index)
            ;
      else if (ch == '/' && peek() == '*')
      {
         for (; this->index < this->size && (prev() != '*' || this->source.at(this->index) != '/'); ++this->index)
            ;

         if (this->index >= this->size)
            this->catcher.insert(err::unterminated_comment);
      }
      else if (ch == '?')
         push_token(TType::question, "?"s);
      else if (ch == ':')
         push_token(TType::colon, ":"s);
      else if (ch == '=' && peek() == '=')
         push_token_ad(TType::equals_equals, "=="s);
      else if (ch == '=')
         push_token(TType::equals, "="s);
      else if (ch == '+' && peek() == '=')
         push_token_ad(TType::plus_equals, "+="s);
      else if (ch == '+' && peek() == '+')
         push_token_ad(TType::plus_plus, "++"s);
      else if (ch == '+')
         push_token(TType::plus, "+"s);
      else if (ch == '-' && peek() == '=')
         push_token_ad(TType::minus_equals, "-="s);
      else if (ch == '-' && peek() == '-')
         push_token_ad(TType::minus_minus, "--"s);
      else if (ch == '-')
         push_token(TType::minus, "-"s);
      else if (ch == '*' && peek() == '=')
         push_token_ad(TType::star_equals, "*="s);
      else if (ch == '*' && peek() == '*' && peek2() == '=')
         push_token_ad2(TType::star_star_equals, "**="s);
      else if (ch == '*' && peek() == '*')
         push_token_ad(TType::star_star, "**"s);
      else if (ch == '*')
         push_token(TType::star, "*"s);
      else if (ch == '/' && peek() == '=')
         push_token_ad(TType::slash_equals, "/="s);
      else if (ch == '/')
         push_token(TType::slash, "/"s);
      else if (ch == '%' && peek() == '=')
         push_token_ad(TType::percent_equals, "%="s);
      else if (ch == '%')
         push_token(TType::percent, "%"s);
      else if (ch == '<' && peek() == '<' && peek2() == '=')
         push_token_ad2(TType::shift_left_equals, "<<="s);
      else if (ch == '<' && peek() == '<')
         push_token_ad(TType::shift_left, "<<"s);
      else if (ch == '<' && peek() == '=')
         push_token_ad(TType::smaller_equals, "<="s);
      else if (ch == '<')
         push_token(TType::smaller, "<"s);
      else if (ch == '>' && peek() == '>' && peek2() == '=')
         push_token_ad2(TType::shift_right_equals, ">>="s);
      else if (ch == '>' && peek() == '>')
         push_token_ad(TType::shift_right, ">>"s);
      else if (ch == '>' && peek() == '=')
         push_token_ad(TType::bigger_equals, ">="s);
      else if (ch == '>')
         push_token(TType::bigger, ">"s);
      else if (ch == '!' && peek() == '=')
         push_token_ad(TType::not_equals, "!="s);
      else if (ch == '!')
         push_token(TType::logical_not, "!"s);
      else if (ch == '~')
         push_token(TType::bitwise_not, "~"s);
      else if (ch == '&' && peek() == '&')
         push_token_ad(TType::logical_and, "&&"s);
      else if (ch == '&' && peek() == '=')
         push_token_ad(TType::bitwise_and_equals, "&="s);
      else if (ch == '&')
         push_token(TType::bitwise_and, "&"s);
      else if (ch == '|' && peek() == '|')
         push_token_ad(TType::logical_or, "||"s);
      else if (ch == '|' && peek() == '=')
         push_token_ad(TType::bitwise_or_equals, "|="s);
      else if (ch == '|')
         push_token(TType::bitwise_or, "|"s);
      else if (ch == '^' && peek() == '=')
         push_token_ad(TType::bitwise_xor_equals, "^="s);
      else if (ch == '^')
         push_token(TType::bitwise_xor, "^"s);
      else if (ch == '.' && peek() == '.' && peek2() == '.')
         push_token_ad2(TType::dot_dot_dot, "..."s);
      else if (ch == '.')
         push_token(TType::dot, "."s);
      else if (ch == ',')
         push_token(TType::comma, ","s);
      else if (ch == ';' && peek() == ';')
         push_token_ad(TType::newline, ";;"s);
      else if (ch == ';')
         push_token(TType::semicolon, ";"s);
      else if (ch == '#' && peek() == '#')
         push_token_ad(TType::hash_hash, "##"s);
      else if (ch == '#' && peek() == '=' && peek2() == '=')
         push_token_ad2(TType::hash_equals, "#=="s);
      else if (ch == '#' && peek() == '!' && peek2() == '=')
         push_token_ad2(TType::hash_not_equals, "#!="s);
      else if (ch == '(')
         push_token(TType::l_paren, "("s);
      else if (ch == ')')
         push_token(TType::r_paren, ")"s);
      else if (ch == '[')
         push_token(TType::l_bracket, "["s);
      else if (ch == ']')
         push_token(TType::r_bracket, "]"s);
      else if (ch == '{')
         push_token(TType::l_brace, "{"s);
      else if (ch == '}')
         push_token(TType::r_brace, "}"s);
      else if (ch == '"')
      {
         advance();
         std::string string;

         for (; this->index < this->size; ++this->index)
         {
            ch = this->source.at(this->index);

            if (ch == '"')
               break;
            
            if (ch == '\\')
            {
               char next = advance();

               if (next == 'n')
                  ch = '\n';
               else if (next == 'r')
                  ch = '\r';
               else if (next == 't')
                  ch = '\t';
               else if (next == '\'')
                  ch = '\'';
               else if (next == '"')
                  ch = '"';
               else if (next == '\\')
                  ch = '\\';
               else if (next == '0')
                  ch = '\0';
               else
                  this->catcher.insert(err::invalid_escape_code);
            }
            string += ch;
         }

         if (this->index >= this->size)
         {
            this->catcher.insert(err::unterminated_string);
            return tokens;
         }
         push_token(TType::string, string);
      }
      else if (ch == '\'')
      {
         ch = advance();
         char next = advance();

         if (ch == '\\')
         {
            if (next == 'n')
               ch = '\n';
            else if (next == 'r')
               ch = '\r';
            else if (next == 't')
               ch = '\t';
            else if (next == '\'')
               ch = '\'';
            else if (next == '"')
               ch = '"';
            else if (next == '\\')
               ch = '\\';
            else if (next == '0')
               ch = '\0';
            else
               this->catcher.insert(err::invalid_escape_code);
            next = advance();
         }
         
         if (next != '\'' || this->index >= this->size)
            this->catcher.insert(err::invalid_char);
         
         push_token(TType::character, std::string(1, ch));
      }
      else if (isalpha(ch) || ch == '#' || ch == '_')
      {
         std::string identifier;
         bool macro = (ch == '#');

         if (macro)
            advance();
         
         for (; this->index < this->size; ++this->index)
         {
            ch = this->source.at(this->index);

            if (!isalnum(ch) && ch != '_')
            {
               --this->index;
               break;
            }
            identifier += ch;
         }
         bool keyword = (keywords.find(identifier) != keywords.end());
         push_token((keyword ? (macro ? TType::macro : TType::keyword) : TType::identifier), identifier);
      }
      else if (isdigit(ch) || ch == '.')
      {
         std::string number;

         bool floating = false;
         bool last_quote = false;

         for (; this->index < this->size; ++this->index)
         {
            ch = this->source.at(this->index);

            if (ch == '\'')
            {
               if (last_quote)
                  this->catcher.insert(err::invalid_quotes);
               last_quote = true;
               continue;
            }
            last_quote = false;

            if (ch == '.')
            {
               if (floating)
                  this->catcher.insert(err::invalid_real_number);
               floating = true;
               number += ch;
               continue;
            }

            if (!isdigit(ch))
            {
               --this->index;
               break;
            }
            number += ch;
         }

         if (last_quote)
            this->catcher.insert(err::invalid_quotes);
         push_token((floating ? TType::real : TType::integer), number);
      }
      else
         this->catcher.insert(err::unexpected_char);
   }
   push_token(TType::eof, "EOF"s);
   return tokens;
}

void Lexer::push_token(TType type, const std::string& lexeme)
{
   Token new_token {type, lexeme};
   this->tokens.push_back(new_token);
}

void Lexer::push_token_ad(TType type, const std::string& lexeme)
{
   Token new_token {type, lexeme};
   this->tokens.push_back(new_token);
   ++this->index;
}

void Lexer::push_token_ad2(TType type, const std::string& lexeme)
{
   Token new_token {type, lexeme};
   this->tokens.push_back(new_token);
   this->index += 2;
}

char Lexer::advance()
{
   if (this->index + 1 >= size)
      return char{};
   ++this->index;
   return this->source.at(this->index);
}

char Lexer::peek() const
{
   if (this->index + 1 >= size)  
      return char{};
   return this->source.at(this->index + 1);
}

char Lexer::peek2() const
{
   if (this->index + 2 >= size)
      return char{};
   return this->source.at(this->index + 2);
}

char Lexer::prev() const
{
   if (this->index == 0)
      return char{};
   return this->source.at(this->index - 1);
}
