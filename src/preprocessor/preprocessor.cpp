#include "preprocessor/preprocessor.hpp"
#include "errors/errors.hpp"
#include <algorithm>

Preprocessor::Preprocessor(Catcher& catcher, std::vector<Token>& tokens, const std::string& file)
   : catcher(catcher), tokens(tokens)
{
   if (!file.empty())
      this->included_files.insert(file);
   this->total_size = this->tokens.size();
}

void Preprocessor::specify_max_macro_depth(size_t max_macro_depth)
{
   this->max_macro_depth = max_macro_depth;
}

void Preprocessor::process()
{
   for (; this->index < this->total_size; ++this->index)
   {
      auto& token = current();

      if (token.type == TType::macro && (token.lexeme == "import" || token.lexeme == "include"))
         handle_importing();
      else if (token.type == TType::macro && (token.lexeme == "define" || token.lexeme == "def" || token.lexeme == "define_line" || token.lexeme == "defl"))
         handle_macro_definition();
      else if (token.type == TType::identifier && this->macros.find(token.lexeme) != this->macros.end())
         handle_using_macro();
      else if (token.type == TType::macro && (token.lexeme == "undef" || token.lexeme == "undefine"))
         handle_deleting_macro();
      else if (is_macro_conditional(token))
         handle_macro_conditionals();
      else if (is_macro_conditional_else_if(token) || (token.type == TType::macro && (token.lexeme == "else" || token.lexeme == "endif" || token.lexeme == "then")))
         this->catcher.insert(err::invalid_mcond_start);
      else if (token.type == TType::hash_hash)
         handle_concatenation();
      else if (token.type == TType::macro_equals || token.type == TType::macro_bang_equals)
         handle_equality_operators();
      
      if (!this->catcher.empty())
         return;
   }

   this->tokens.erase(std::remove_if(this->tokens.begin(), this->tokens.end(), [](const Token& t) -> bool
   {
      return t.type == TType::skip || t.type == TType::newline;
   }), this->tokens.end());
}

void Preprocessor::handle_macro_definition()
{
   this->macro_depth = 0;
   auto& token = current();
   bool define_line = (token.lexeme == "define_line" || token.lexeme == "defl");
   auto& name_token = skip();

   if (name_token.type != TType::identifier)
   {
      this->catcher.insert(err::expected_ident_macro_def);
      return;
   }

   if (this->macros.find(name_token.lexeme) != this->macros.end())
   {
      this->catcher.insert(err::macro_exists);
      return;
   }

   token = skip();
   bool params = (token.type == TType::l_paren);

   if (this->index + 1 < this->total_size)
      token = this->tokens.at(this->index + 1);

   if (params && (token.type == TType::eof || token.type == TType::r_paren))
   {
      this->catcher.insert(err::invalid_macro_params);
      return;
   }

   if (params && token.type == TType::dot_dot_dot)
   {
      skip();
      token = skip();

      if (token.type != TType::r_paren)
      {
         this->catcher.insert(err::expected_r_paren_variadic_macro);
         return;
      }

      std::vector<Token> values;
      Token variadic {TType::newline, "variadic"};
      values.push_back(variadic);

      token = skip();
      if (token.type != TType::equals)
      {
         this->catcher.insert(err::expected_equals_macro_def);
         return;
      }
      token = skip();

      TType end = (define_line ? TType::newline : TType::semicolon);
      while (token.type != end && token.type != TType::eof)
      {
         Token copied {token.type, token.lexeme};
         values.push_back(copied);
         token = skip();
      }

      if (values.size() - 1 == 0)
      {
         this->catcher.insert(err::invalid_macro_body);
         return;
      }
      this->macros.insert({name_token.lexeme, values});
   }
   else if (params)
   {
      skip();
      std::vector<Token> values;

      Token separator {TType::newline, "separator"};
      values.push_back(separator);

      size_t arg_count = 0;
      token = current();

      while (token.type == TType::identifier)
      {
         Token new_token {token.type, token.lexeme};
         values.push_back(new_token);

         ++arg_count;
         token = skip();

         if (token.type != TType::comma && token.type != TType::r_paren)
         {
            this->catcher.insert(err::expected_comma_or_r_paren);
            return;
         }
         skip();

         if (token.type == TType::r_paren)
            break;
         token = current();
      }
      values.push_back(separator);
      token = current();

      if (token.type != TType::equals)
      {
         this->catcher.insert(err::expected_equals_macro_def);
         return;
      }
      token = skip();

      TType end = (define_line ? TType::newline : TType::semicolon);
      while (token.type != end && token.type != TType::eof)
      {
         Token copied {token.type, token.lexeme};
         values.push_back(copied);
         token = skip();
      }

      if (values.size() - 2 - arg_count == 0)
      {
         this->catcher.insert(err::invalid_macro_body);
         return;
      }
      this->macros.insert({name_token.lexeme, values});
   }
   else
   {
      token = current();

      if (token.type == TType::semicolon)
      {
         token.type = TType::skip;
         skip();
         --this->index;

         std::vector<Token> values;
         Token fake {TType::skip, ""};
         values.push_back(fake);

         this->macros.insert({name_token.lexeme, values});
         return;
      }

      if (token.type != TType::equals)
      {
         this->catcher.insert(err::expected_equals_macro_def);
         return;
      }
      token = skip();

      std::vector<Token> values;
      TType end = (define_line ? TType::newline : TType::semicolon);

      while (token.type != end && token.type != TType::eof)
      {
         Token copied {token.type, token.lexeme};
         values.push_back(copied);
         token = skip();
      }

      if (values.empty())
      {
         this->catcher.insert(err::invalid_macro_body);
         return;
      }
      this->macros.insert({name_token.lexeme, values});
   }
   token = current();

   if (!define_line && token.type != TType::semicolon)
   {
      this->catcher.insert(err::statement_semicolon);
      return;
   }
   token.type = TType::skip;
   skip();
   --this->index;
}

void Preprocessor::handle_using_macro()
{
   ++this->macro_depth;

   if (this->macro_depth >= this->max_macro_depth)
   {
      this->catcher.insert(err::infinite_macro_loop);
      return;
   }

   auto& token = current();
   auto& definition = this->macros.at(token.lexeme);

   token = skip();
   bool args = (token.type == TType::l_paren);

   if (this->index + 1 < this->total_size)
      token = this->tokens.at(this->index + 1);
   
   if (args && (token.type == TType::eof || token.type == TType::r_paren))
   {
      this->catcher.insert(err::invalid_macro_call);
      return;
   }

   if (args && definition.size() > 0 && definition.at(0).type == TType::newline && definition.at(0).lexeme == "variadic")
   {
      auto copied = definition;
      token = skip();
      Token parameter {TType::string, ""};

      size_t param_depth = 1;

      while (this->index < this->total_size)
      {
         while (token.type != TType::comma && token.type != TType::eof)
         {
            if (token.type == TType::l_paren)
               ++param_depth;

            if (token.type == TType::r_paren)
            {
               --param_depth;
               if (param_depth == 0)
                  break;
            }
            parameter.lexeme += token.lexeme + " ";
            token = skip();
         }

         if (token.type != TType::comma && token.type != TType::r_paren)
         {
            this->catcher.insert(err::expected_comma_or_r_paren);
            return;
         }

         if (token.type == TType::r_paren)
         {
            token.type = TType::skip;
            skip();
            --this->index;
            break;
         }
         token = skip();
      }
      parameter.lexeme.pop_back();

      for (size_t i = 1; i < copied.size(); ++i)
      {
         auto& t = copied.at(i);

         if (t.lexeme == "...")
            t = parameter;
      }
      this->tokens.insert(this->tokens.begin() + this->index + 1, copied.begin() + 1, copied.end());
      this->total_size = this->tokens.size();
   }
   else if (args)
   {
      auto copied = definition;
      token = skip();
      std::vector<std::vector<Token>> params;

      size_t param_depth = 1;

      while (this->index < this->total_size)
      {
         std::vector<Token> param;

         while (token.type != TType::comma && token.type != TType::eof)
         {
            if (token.type == TType::l_paren)
               ++param_depth;

            if (token.type == TType::r_paren)
            {
               --param_depth;
               if (param_depth == 0)
                  break;
            }

            Token copied {token.type, token.lexeme};
            param.push_back(copied);
            token = skip();
         }
         params.push_back(param);

         if (token.type != TType::comma && token.type != TType::r_paren)
         {
            this->catcher.insert(err::expected_comma_or_r_paren);
            return;
         }

         if (token.type == TType::r_paren)
         {
            token.type = TType::skip;
            skip();
            --this->index;
            break;
         }
         token = skip();
      }

      std::unordered_map<std::string, size_t> translations;
      size_t param_count = 0;

      for (size_t i = 1; i < copied.size(); ++i)
      {
         auto& t = copied.at(i);
         if (t.type == TType::newline)
            break;

         ++param_count;
         translations.insert({t.lexeme, i - 1});
         t.type = TType::skip;
      }

      if (param_count != params.size() || (copied.size() > 0 && (copied.at(0).type != TType::newline || copied.at(0).lexeme != "separator")))
      {
         this->catcher.insert(err::invalid_arg_count);
         return;
      }

      for (size_t i = param_count + 2; i < copied.size(); ++i)
      {
         auto& t = copied.at(i);

         if (translations.find(t.lexeme) == translations.end())
            continue;
         
         if (t.type == TType::identifier)
         {
            t.type = TType::skip;

            auto& replacement = params.at(translations.at(t.lexeme));
            copied.insert(copied.begin() + i + 1, replacement.begin(), replacement.end());
         }
         else if (t.type == TType::string)
         {
            auto& replacement = params.at(translations.at(t.lexeme));
            t.lexeme.clear();

            for (auto& rep : replacement)
               t.lexeme += rep.lexeme + " ";
            t.lexeme.pop_back();
         }
      }
      this->tokens.insert(this->tokens.begin() + this->index + 1, copied.begin() + 2 + param_count, copied.end());
      this->total_size = this->tokens.size();
   }
   else
   {
      token = definition.at(0);

      if (token.type == TType::skip)
      {
         this->catcher.insert(err::called_empty_macro);
         return;
      }

      if (token.type == TType::newline && (token.lexeme == "separator" || token.lexeme == "variadic"))
      {
         this->catcher.insert(err::invalid_arg_count);
         return;
      }

      this->tokens.insert(this->tokens.begin() + this->index, definition.begin() + 1, definition.end());
      this->total_size = this->tokens.size();
      this->index -= 2;
   }
}

void Preprocessor::handle_deleting_macro()
{
   auto& token = skip();
   if (token.type != TType::identifier)
   {
      this->catcher.insert(err::invalid_undefine);
      return;
   }

   this->macros.erase(token.lexeme);
   auto& end = skip();

   if (end.type != TType::semicolon)
   {
      this->catcher.insert(err::statement_semicolon);
      return;
   }
   end.type = TType::skip;
}

void Preprocessor::handle_importing()
{

}

void Preprocessor::handle_file()
{

}

void Preprocessor::handle_macro_conditionals()
{

}

void Preprocessor::handle_boolean_expressions()
{

}

void Preprocessor::handle_concatenation()
{
   if (this->index == 0 || this->index + 1 >= this->total_size)
   {
      this->catcher.insert(err::invalid_concatenation_macro);
      return;
   }
   size_t op_index = this->index;

   auto& left  = this->tokens.at(op_index - 1);
   auto& right = this->tokens.at(op_index + 1);

   right.type = TType::string;
   right.lexeme = left.lexeme + right.lexeme;

   this->tokens.erase(this->tokens.begin() + op_index);
   this->tokens.erase(this->tokens.begin() + op_index - 1);

   this->index = op_index - 1;
   this->total_size = this->tokens.size();
}

void Preprocessor::handle_equality_operators()
{
   if (this->index == 0 || this->index + 1 >= this->total_size)
   {
      this->catcher.insert(err::invalid_concatenation_macro);
      return;
   }
   size_t op_index = this->index;
   bool negative = (current().type == TType::macro_bang_equals);

   auto& left  = this->tokens.at(op_index - 1);
   auto& right = this->tokens.at(op_index + 1);

   bool result = (left.lexeme == right.lexeme);
   result = (negative ? !result : result);

   right.type = TType::integer;
   right.lexeme = std::to_string(result);

   this->tokens.erase(this->tokens.begin() + op_index);
   this->tokens.erase(this->tokens.begin() + op_index - 1);

   this->index = op_index - 1;
   this->total_size = this->tokens.size();
}

Token& Preprocessor::current()
{
   return this->tokens.at(this->index);
}

Token& Preprocessor::skip()
{
   this->tokens.at(this->index).type = TType::skip;
   advance();
   return this->tokens.at(this->index);
}

void Preprocessor::advance()
{
   if (this->index + 1 < this->total_size)
      ++this->index;
}

bool Preprocessor::is_macro_conditional(const Token& token) const
{
   return token.type == TType::macro && (token.lexeme == "if" || token.lexeme == "ifn" || token.lexeme == "ifdef" || token.lexeme == "ifndef");
}

bool Preprocessor::is_macro_conditional_else_if(const Token& token) const
{
   return token.type == TType::macro && (token.lexeme == "elif" || token.lexeme == "elifn" || token.lexeme == "elifdef" || token.lexeme == "elifndef");
}

int Preprocessor::get_operator_precedence(TType type) const
{
   switch (type)
   {
      case TType::bang: return 5;
      case TType::smaller: case TType::smaller_equal: case TType::bigger: case TType::bigger_equal: return 4;
      case TType::equals_equals: case TType::bang_equal: return 3;
      case TType::and_: return 2;
      case TType::or_: return 1;
      default: return 0;
   }
}

bool Preprocessor::has_higher_precedence(TType first, TType second)
{
   return get_operator_precedence(first) >= get_operator_precedence(second);
}
