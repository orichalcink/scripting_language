#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "errors/catcher.hpp"
#include "lexer/tokens.hpp"
#include <unordered_map>
#include <unordered_set>

enum class CType : std::int8_t
{ true_, false_, evaluated, evaluating };

class Preprocessor
{
public:
   Preprocessor(Catcher& catcher, std::vector<Token>& tokens, const std::string& file);
   ~Preprocessor() = default;

   void specify_max_macro_depth(size_t max_macro_depth);

   void process();

private:
   Catcher& catcher;
   std::vector<Token>& tokens;

   std::unordered_map<std::string, std::vector<Token>> macros;
   std::unordered_set<std::string> included_files;
   size_t index = 0;
   size_t total_size = 0;

   size_t macro_depth = 0;
   size_t max_macro_depth = 32;

   void evaluate_token();
   void handle_macro_definition();
   void handle_using_macro();
   void handle_deleting_macro();
   void handle_importing();
   void handle_file();
   void handle_macro_conditionals();
   void handle_boolean_expressions();
   void handle_concatenation();
   void handle_equality_operators();
   void handle_errors();
   void handle_logging();

   Token& current();
   Token& skip();
   void advance();

   bool is_macro_conditional(const Token& token) const;
   bool is_macro_conditional_else_if(const Token& token) const;
   int get_operator_precedence(TType type) const;
   bool has_higher_precedence(TType first, TType second);
};

#endif // PREPROCESSOR_H
