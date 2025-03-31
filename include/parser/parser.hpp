#ifndef PARSER_HPP
#define PARSER_HPP

#include "errors/catcher.hpp"
#include "lexer/tokens.hpp"
#include "parser/ast.hpp"

class Parser
{
public:
   Parser(Catcher& catcher, std::vector<Token>& tokens);
   ~Parser() = default;

   Program& parse();

private:
   Catcher& catcher;
   std::vector<Token>& tokens;
   Program program;
   size_t index = 0;

   Stmt parse_stmt();
   Stmt parse_var_declaration();
   Stmt parse_type();

   Stmt parse_compound_bitwise_expr();
   Stmt parse_compound_shift_expr();
   Stmt parse_compound_additive_expr();
   Stmt parse_compound_multiplicative_expr();
   Stmt parse_compound_exponentiation_expr();
   Stmt parse_assignment_expr();
   Stmt parse_ternary_expr();
   Stmt parse_logical_or_expr();
   Stmt parse_logical_and_expr();
   Stmt parse_bitwise_or_expr();
   Stmt parse_bitwise_xor_expr();
   Stmt parse_bitwise_and_expr();
   Stmt parse_equality_expr();
   Stmt parse_relational_expr();
   Stmt parse_shift_expr();
   Stmt parse_additive_expr();
   Stmt parse_multiplicative_expr();
   Stmt parse_exponentiation_expr();
   Stmt parse_unary_expr();
   Stmt parse_increment_suffix();
   Stmt parse_primary_expr();

   void advance();
   bool is(TType type) const;
   bool is_type() const;
   Token& current();
};

#endif // PARSER_HPP
