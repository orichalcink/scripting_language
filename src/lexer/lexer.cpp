#include "lexer/lexer.hpp"
#include "errors/errors.hpp"
#include "lexer/keywords.hpp"

Lexer::Lexer(Catcher& catcher, std::string& source)
   : catcher(catcher), source(source) {}

std::vector<Token>& Lexer::tokenize()
{
   const auto source_size = this->source.size();

   for (; this->index < source_size; ++this->index)
   {
      char ch = this->source.at(this->index);

      if (ch == '\n')
         push_token(TType::newline, "\n");
      else if (isspace(ch))
         continue;
      else if (ch == ';' && peek() == ';')
         push_token_ad(TType::newline, ";;");
      else if (ch == ';')
         push_token(TType::semicolon, ";");
      else if (ch == '=' && peek() == '=')
         push_token_ad(TType::equals_equals, "==");
      else if (ch == '=')
         push_token(TType::equals, "=");
      else if (ch == '<' && peek() == '=')
         push_token_ad(TType::smaller_equal, "<=");
      else if (ch == '<')
         push_token(TType::smaller, "<");
      else if (ch == '>' && peek() == '=')
         push_token_ad(TType::bigger_equal, ">=");
      else if (ch == '>')
         push_token(TType::bigger, ">");
      else if (ch == '!' && peek() == '=')
         push_token_ad(TType::bang_equal, "!=");
      else if (ch == '!')
         push_token(TType::bang, "!");
      else if (ch == '&' && peek() == '&')
         push_token_ad(TType::and_, "&&");
      else if (ch == '|' && peek() == '|')
         push_token_ad(TType::or_, "||");
      else if (ch == '(')
         push_token(TType::l_paren, "(");
      else if (ch == ')')
         push_token(TType::r_paren, ")");
      else if (ch == ',')
         push_token(TType::comma, ",");
      else if (ch == '/' && peek() == '/')
         for (; this->index < source_size && this->source.at(this->index) != '\n'; ++this->index)
            ;
      else if (ch == '/' && peek() == '*')
      {
         for (; this->index < source_size && (prev() != '*' || this->source.at(this->index) != '/'); ++this->index)
            ;

         if (this->index >= source_size)
            this->catcher.insert(err::unterminated_comment);
      }
      else if (ch == '"')
      {
         advance();
         std::string string;

         for (; this->index < source_size; ++this->index)
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

         if (this->index >= source_size)
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
         
         if (next != '\'' || this->index >= source_size)
            this->catcher.insert(err::invalid_char);
         
         push_token(TType::character, std::string(1, ch));
      }
      else if (isalpha(ch) || ch == '#')
      {
         std::string identifier;
         bool macro = (ch == '#');

         if (macro)
            advance();
         
         for (; this->index < source_size; ++this->index)
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

         for (; this->index < source_size; ++this->index)
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
   push_token(TType::eof, "EOF");
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

char Lexer::advance()
{
   if (this->index + 1 >= this->source.size())
      return 0;
   ++this->index;
   return this->source.at(this->index);
}

char Lexer::peek() const
{
   if (this->index + 1 >= this->source.size())  
      return 0;
   return this->source.at(this->index + 1);
}

char Lexer::prev() const
{
   if (this->index == 0)
      return 0;
   return this->source.at(this->index - 1);
}
