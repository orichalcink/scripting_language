#include "preprocessor/preprocessor.hpp"
#include "errors/errors.hpp"
#include "io/files.hpp"
#include "lexer/lexer.hpp"
#include "config/version.hpp"
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <stack>

Preprocessor::Preprocessor(Catcher& catcher, std::vector<Token>& tokens, const std::string& file, bool skip_macros)
   : catcher(catcher), tokens(tokens)
{
   this->total_size = this->tokens.size();

   if (!file.empty())
   {
      this->included_files.insert(file);

      if (skip_macros)
         return;
      Token file_token {TType::string, file};
      this->macros.insert({"__FILE__", {file_token}});
   }
   else
   {
      if (skip_macros)
         return;
      Token file_token {TType::string, "REPL"};
      this->macros.insert({"__FILE__", {file_token}});
   }

   Token skip  {TType::skip, ""};
   Token token {TType::integer, std::to_string(version::version)};
   this->macros.insert({"__VERSION__", {token}});

   token.lexeme = std::to_string(version::major);
   this->macros.insert({"__VERSION_MAJOR__", {token}});

   token.lexeme = std::to_string(version::minor);
   this->macros.insert({"__VERSION_MINOR__", {token}});

   token.lexeme = std::to_string(version::patch);
   this->macros.insert({"__VERSION_PATCH__", {token}});

   token = {TType::string, version::string};
   this->macros.insert({"__VERSION_STR__", {token}});

   auto time = std::chrono::high_resolution_clock::now();

   token = {TType::integer, std::to_string(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count())};
   this->macros.insert({"__EPOCH__", {token}});

   token = {TType::integer, std::to_string(time.time_since_epoch().count())};
   this->macros.insert({"__EPOCH_NS__", {token}});

   auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   std::tm now_tm = *std::localtime(&now);

   std::ostringstream oss;
   oss << std::put_time(&now_tm, "%Y-%m-%d");

   token = {TType::string, oss.str()};
   this->macros.insert({"__DATE__", {token}});

   oss.str("");
   oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

   token = {TType::string, oss.str()};
   this->macros.insert({"__DATETIME__", {token}});

   oss.str("");
   oss << std::put_time(&now_tm, "%H:%M:%S");

   token = {TType::string, oss.str()};
   this->macros.insert({"__TIME__", {token}});

   #if defined(_WIN64) || defined(_WIN32)
      this->macros.insert({"__WIN__", {skip}});
      token = {TType::string, "Windows"};
      this->macros.insert({"__OS__", {token}});

      #if defined(_WIN64)
         this->macros.insert({"__64BIT__", {skip}});
      #else
         this->macros.insert({"__32BIT__", {skip}});
      #endif
   #elif defined(__linux__)
      this->macros.insert({"__LINUX__", {skip}});
      token = {TType::string, "Linux"};
      this->macros.insert({"__OS__", {token}});

      #if defined(__x86_64__) || defined(_M_X64)
         this->macros.insert({"__64BIT__", {skip}});
      #else
         this->macros.insert({"__32BIT__", {skip}});
      #endif
   #elif defined(__APPLE__)
      this->macros.insert({"__MACOS__", {skip}});
      token = {TType::string, "MacOS"};
      this->macros.insert({"__OS__", {token}});

      #if defined(__x86_64__)
         this->macros.insert({"__64BIT__", {skip}});
      #else
         this->macros.insert({"__32BIT__", {skip}});
      #endif
   #endif

}

void Preprocessor::specify_max_macro_depth(size_t max_macro_depth)
{
   this->max_macro_depth = max_macro_depth;
}

void Preprocessor::process()
{
   for (; this->index < this->total_size; ++this->index)
   {
      evaluate_token();
      if (!this->catcher.empty())
         return;
   }

   this->tokens.erase(std::remove_if(this->tokens.begin(), this->tokens.end(), [](const Token& t) -> bool
   {
      return t.type == TType::skip || t.type == TType::newline || t.type == TType::eoi;
   }), this->tokens.end());
}

void Preprocessor::evaluate_token()
{
   auto& token = current();
   bool used_macro = false;

   if (token.type == TType::macro && (token.lexeme == "import" || token.lexeme == "include"))
      handle_importing();
   else if (token.type == TType::macro && (token.lexeme == "def" || token.lexeme == "defl"))
      handle_macro_definition();
   else if (token.type == TType::identifier && this->macros.find(token.lexeme) != this->macros.end())
   {
      handle_using_macro();
      used_macro = true;
   }
   else if (token.type == TType::macro && token.lexeme == "undef")
      handle_deleting_macro();
   else if (token.type == TType::macro && token.lexeme == "if")
      handle_macro_conditionals();
   else if (token.type == TType::macro && (token.lexeme == "elif" || token.lexeme == "else" || token.lexeme == "endif"))
      this->catcher.insert(err::invalid_mcond_start);
   else if (token.type == TType::hash_hash)
      handle_concatenation();
   else if (token.type == TType::macro_equals || token.type == TType::macro_bang_equals)
      handle_equality_operators();
   else if (token.type == TType::macro && token.lexeme == "error")
      handle_errors();
   else if (token.type == TType::macro && (token.lexeme == "log" || token.lexeme == "logl"))
      handle_logging();
   else if (token.type == TType::eoi && this->macros.find("__FILE__") != this->macros.end())
      this->macros.at("__FILE__").at(0).lexeme = token.lexeme;

   if (!used_macro)
      this->macro_depth = 0;
}

void Preprocessor::handle_macro_definition()
{
   auto& token = current();
   bool define_line = (token.lexeme == "defl");
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

   if (params)
   {
      skip();
      std::vector<Token> values;

      Token separator {TType::newline, "separator"};
      values.push_back(separator);

      size_t arg_count = 0;
      bool variadic = false;
      token = current();

      while (token.type == TType::identifier || token.type == TType::dot_dot_dot)
      {
         Token new_token {token.type, token.lexeme};
         values.push_back(new_token);

         if (token.type == TType::dot_dot_dot)
         {
            if (variadic)
            {
               this->catcher.insert(err::invalid_variadic_macro);
               return;
            }
            variadic = true;
         }

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

      if (variadic && values.back().type != TType::dot_dot_dot)
      {
         this->catcher.insert(err::invalid_variadic_macro);
         return;
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
         values.push_back(token);
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
         values.push_back(token);
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

   if (args)
   {
      auto copied = definition;
      token = skip();
      std::vector<std::vector<Token>> params;
      std::vector<Token> variadic_params;

      size_t param_depth = 1;
      size_t l_index = 1;
      bool variadic = false;

      while (this->index < this->total_size)
      {
         std::vector<Token> param;

         bool valid = (l_index <= copied.size());

         if (valid && copied.at(l_index).type == TType::dot_dot_dot)
            variadic = true;
         else if (valid && copied.at(l_index).type == TType::newline)
            l_index = copied.size();

         ++l_index;

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

            if (variadic)
               variadic_params.push_back(token);
            else
               param.push_back(token);
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

      bool valid_arg_count = (variadic ? param_count <= params.size() : param_count == params.size());
      bool is_parametrized = (copied.size() > 0 && (copied.front().type == TType::newline && copied.front().lexeme == "separator"));

      if (!valid_arg_count || !is_parametrized)
      {
         this->catcher.insert(err::invalid_arg_count);
         return;
      }

      for (size_t i = param_count + 2; i < copied.size(); ++i)
      {
         auto& t = copied.at(i);

         if (translations.find(t.lexeme) == translations.end())
            continue;

         if (variadic && t.type == TType::dot_dot_dot)
         {
            t.type = TType::skip;
            copied.insert(copied.begin() + i + 1, variadic_params.begin(), variadic_params.end());
         }
         else if (variadic && t.type == TType::string && t.lexeme == "...")
         {
            t.lexeme.clear();

            for (auto& v : variadic_params)
               t.lexeme += v.lexeme + " ";
            t.lexeme.pop_back();
         }
         else if (t.type == TType::identifier)
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

      if (token.type == TType::newline && token.lexeme == "separator")
      {
         this->catcher.insert(err::invalid_arg_count);
         return;
      }

      this->tokens.insert(this->tokens.begin() + this->index, definition.begin() + 1, definition.end());
      this->total_size = this->tokens.size();
      this->index -= 1;
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
   auto& token = current();
   bool include_guard = (token.lexeme == "import");
   
   token = skip();
   if (token.type == TType::identifier && this->macros.find(token.lexeme) != this->macros.end())
   {
      handle_using_macro();
      ++this->index;
      token = current();
   }

   if (token.type != TType::string)
   {
      this->catcher.insert(err::expected_file);
      return;
   }
   std::string& file = token.lexeme;
   
   std::vector<std::string> files;
   files.push_back(file);
   token = skip();

   while (token.type == TType::comma)
   {
      token = skip();
      if (token.type == TType::identifier && this->macros.find(token.lexeme) != this->macros.end())
      {
         handle_using_macro();
         ++this->index;
         token = current();
      }

      if (token.type != TType::string)
      {
         this->catcher.insert(err::expected_file);
         return;
      }
      files.push_back(token.lexeme);
      token = skip();
   }

   if (token.type != TType::semicolon)
   {
      this->catcher.insert(err::statement_semicolon);
      return;
   }
   token.type = TType::skip;
   skip();
   --this->index;

   for (auto& f : files)
      handle_file(f, include_guard);
}

void Preprocessor::handle_file(const std::string& file, bool include_guard)
{
   if (!is_file(file))
   {
      this->catcher.insert(err::import_invalid_file);
      return;
   }
   bool contains = (this->included_files.find(file) != this->included_files.end());

   if (include_guard && contains)
      return;
   
   if (!contains)
      this->included_files.insert(file);
   
   std::string input = read_file(this->catcher, file);

   if (!this->catcher.empty())
      return;
   
   Lexer lexer (this->catcher, input);
   auto& tokens = lexer.tokenize();

   if (!this->catcher.empty())
      return;

   if (this->macros.find("__FILE__") != this->macros.end())
   {
      tokens.back().type = TType::eoi;
      tokens.back().lexeme = this->macros.at("__FILE__").at(0).lexeme;
      this->macros.at("__FILE__").at(0).lexeme = file;
   }
   else tokens.back().type = TType::skip;

   this->tokens.insert(this->tokens.begin() + this->index , tokens.begin(), tokens.end());
   this->total_size = this->tokens.size();
   --this->index;
}

void Preprocessor::handle_macro_conditionals()
{
   auto& token = skip();

   bool result = handle_boolean_expressions();
   CType condition = (result ? CType::true_ : CType::false_);
   size_t depth = 0;

   while (token.type != TType::eof && (token.type != TType::macro || token.lexeme != "endif" || depth > 0))
   {
      if (token.type == TType::macro && token.lexeme == "elif")
      {
         if (condition == CType::false_ && depth == 0)
         {
            token.type = TType::skip;
            skip();
            --this->index;
            handle_macro_conditionals();
            return;
         }
         else if (condition != CType::false_)
            condition = CType::evaluated;
      }

      if (depth == 0 && token.type == TType::macro && token.lexeme == "else")
      {
         condition = (condition == CType::false_ ? CType::true_ : CType::evaluated);

         if (condition == CType::true_)
            token = skip();
      }

      if (condition == CType::true_)
      {
         evaluate_token();
         ++this->index;
         token = current();
      }
      else
      {
         if (token.type == TType::macro && token.lexeme == "if")
            ++depth;
         else if (depth > 0 && token.type == TType::macro && token.lexeme == "endif")
            --depth;
         token = skip();
      }

      if (!this->catcher.empty())
         return;
   }

   if (token.type == TType::eof)
   {
      this->catcher.insert(err::mcond_endif);
      return;
   }
   token.type = TType::skip;
   skip();
   --this->index;
}

bool Preprocessor::handle_boolean_expressions()
{
   auto& token = current();
   std::stack<Token> operators;
   std::stack<Token> output;

   while (token.type != TType::eof && token.type != TType::newline)
   {
      if (get_operator_precedence(token.type))
      {
         while (!operators.empty())
         {
            auto& top = operators.top();

            if (!has_higher_precedence(token.type, top.type))
            {
               operators.pop();
               output.push(top);
            }
            else break;
         }
         operators.push(token);
      }
      else if (token.type == TType::l_paren)
         operators.push(token);
      else if (token.type == TType::r_paren)
      {
         while (!operators.empty())
         {
            auto& op = operators.top();

            if (op.type == TType::l_paren)
            {
               operators.pop();
               break;
            }
            output.push(op);
            operators.pop();
         }
      }
      else output.push(token);

      token = skip();
   }

   if (token.type == TType::eof)
   {
      this->catcher.insert(err::invalid_mcond);
      return false;
   }

   skip();
   std::stack<Token> reversed;

   while (!operators.empty())
   {
      auto& op = operators.top();
      if (op.type == TType::l_paren)
      {
         this->catcher.insert(err::mcond_mismatched_parentheses);
         return false;
      }
      operators.pop();
      output.push(op);
   }

   while (!output.empty())
   {
      auto& token = output.top();
      output.pop();
      reversed.push(token);
   }

   std::stack<long double> evaluated;

   while (!reversed.empty())
   {
      auto& token = reversed.top();
      reversed.pop();

      if (token.type == TType::identifier)
      {
         if (this->macros.find(token.lexeme) == this->macros.end())
         {
            evaluated.push(0.0);
            continue;
         }

         auto& macro_body = this->macros.at(token.lexeme);

         if (macro_body.size() == 1 && macro_body.at(0).type == TType::skip)
         {
            evaluated.push(1.0);
            continue;
         }

         for (const auto& t : macro_body)
            reversed.push(t);
      }
      else if (token.type == TType::real || token.type == TType::integer)
      {
         long double result = 0.0;
         try
         {
            result = std::stold(token.lexeme);
         }
         catch (...)
         {
            this->catcher.insert(err::could_not_convert_number);
            return false;
         }
         evaluated.push(result);
      }
      else if (token.type == TType::bang)
      {
         if (evaluated.size() == 0)
         {
            this->catcher.insert(err::invalid_bool_expr);
            return false;
         }

         long double a = evaluated.top();
         long double result = (a == 0.0 ? 1.0 : 0.0);
         evaluated.pop();
         evaluated.push(result);
      }
      else
      {
         if (evaluated.size() < 2)
         {
            this->catcher.insert(err::invalid_bool_expr);
            return false;
         }

         long double b = evaluated.top();
         evaluated.pop();
         long double a = evaluated.top();
         evaluated.pop();

         long double result = 0.0;
         switch (token.type)
         {
         case TType::and_:
            result = (a && b);
            break;
         case TType::or_:
            result = (a || b);
            break;
         case TType::equals_equals:
            result = (a == b);
            break;
         case TType::bang_equal:
            result = (a != b);
            break;
         case TType::smaller:
            result = (a < b);
            break;
         case TType::smaller_equal:
            result = (a <= b);
            break;
         case TType::bigger:
            result = (a > b);
            break;
         case TType::bigger_equal:
            result = (a >= b);
            break;
         default:
            result = false;
            this->catcher.insert(err::unexpected_token_mcond);
         }
         evaluated.push(result);
      }
   }
   return static_cast<bool>(evaluated.top());
}

void Preprocessor::handle_concatenation()
{
   if (this->index < 2)
   {
      this->catcher.insert(err::invalid_concatenation_macro);
      return;
   }
   size_t op_index = this->index;

   auto& left  = this->tokens.at(op_index - 2);
   auto& right = this->tokens.at(op_index - 1);

   left.type = TType::string;
   left.lexeme = left.lexeme + right.lexeme;

   this->tokens.erase(this->tokens.begin() + op_index);
   this->tokens.erase(this->tokens.begin() + op_index - 1);

   this->index = op_index - 3;
   this->total_size = this->tokens.size();
}

void Preprocessor::handle_equality_operators()
{
   if (this->index < 2)
   {
      this->catcher.insert(err::invalid_concatenation_macro);
      return;
   }
   size_t op_index = this->index;
   bool negative = (current().type == TType::macro_bang_equals);

   auto& left  = this->tokens.at(op_index - 2);
   auto& right = this->tokens.at(op_index - 1);

   bool result = (left.lexeme == right.lexeme);
   result = (negative ? !result : result);

   left.type = TType::integer;
   left.lexeme = std::to_string(result);

   this->tokens.erase(this->tokens.begin() + op_index);
   this->tokens.erase(this->tokens.begin() + op_index - 1);

   this->index = op_index - 3;
   this->total_size = this->tokens.size();
}

void Preprocessor::handle_errors()
{
   auto& error = skip();

   if (error.type != TType::string)
   {
      this->catcher.insert(err::expected_string_after_error);
      return;
   }
   auto& end = skip();

   if (end.type != TType::semicolon)
   {
      this->catcher.insert(err::statement_semicolon);
      return;
   }
   end.type = TType::skip;
   skip();
   --this->index;
   this->catcher.insert(error.lexeme.c_str());
}

void Preprocessor::handle_logging()
{
   TType end = (current().lexeme == "log" ? TType::semicolon : TType::newline);
   auto& token = skip();
   std::string log;

   while (token.type != end && token.type != TType::eof)
   {
      evaluate_token();
      token = current();

      if (token.type == end || token.type == TType::eof)
         break;

      if (token.type != TType::skip && token.type != TType::newline)
         log += token.lexeme;
      skip();
   }

   if (end == TType::semicolon && token.type != TType::semicolon)
   {
      this->catcher.insert(err::statement_semicolon);
      return;
   }
   token.type = TType::skip;
   skip();

   --this->index;
   std::cout << log << std::endl;
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
