#include "parser/parser.hpp"
#include "errors/errors.hpp"

Parser::Parser(Catcher& catcher, std::vector<Token>& tokens)
   : catcher(catcher), tokens(tokens) {}

Program& Parser::parse()
{
   while (!is(TType::eof))
   {
      this->program.statements.push_back(std::move(parse_compound_bitwise_expr()));

      if (!catcher.empty())
         return this->program;
   }
   return this->program;
}

Stmt Parser::parse_compound_bitwise_expr()
{
   auto left = parse_compound_shift_expr();

   while (is(TType::bitwise_and_equals) || is(TType::bitwise_xor_equals) || is(TType::bitwise_or_equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_compound_shift_expr();
      left = std::make_unique<AssignmentExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_compound_shift_expr()
{
   auto left = parse_compound_additive_expr();

   while (is(TType::shift_left_equals) || is(TType::shift_right_equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_compound_additive_expr();
      left = std::make_unique<AssignmentExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_compound_additive_expr()
{
   auto left = parse_compound_multiplicative_expr();

   while (is(TType::plus_equals) || is(TType::minus_equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_compound_multiplicative_expr();
      left = std::make_unique<AssignmentExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_compound_multiplicative_expr()
{
   auto left = parse_compound_exponentiation_expr();

   while (is(TType::star_equals) || is(TType::slash_equals) || is(TType::percent_equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_compound_exponentiation_expr();
      left = std::make_unique<AssignmentExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_compound_exponentiation_expr()
{
   auto left = parse_assignment_expr();

   while (is(TType::star_star_equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_assignment_expr();
      left = std::make_unique<AssignmentExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_assignment_expr()
{
   auto left = parse_ternary_expr();

   while (is(TType::equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_ternary_expr();
      left = std::make_unique<AssignmentExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_ternary_expr()
{
   auto boolean_expr = parse_logical_or_expr();

   if (is(TType::question))
   {
      advance();
      auto left = parse_compound_bitwise_expr();

      if (!is(TType::colon))
      {
         this->catcher.insert(err::expected_colon_ternary);
         return boolean_expr;
      }
      advance();

      auto right = parse_compound_bitwise_expr();
      boolean_expr = std::make_unique<TernaryExpr>(boolean_expr, left, right);
   }
   return boolean_expr;
}

Stmt Parser::parse_logical_or_expr()
{
   auto left = parse_logical_and_expr();

   while (is(TType::logical_or))
   {
      TType op = current().type;
      advance();

      auto right = parse_logical_and_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_logical_and_expr()
{
   auto left = parse_bitwise_or_expr();

   while (is(TType::logical_and))
   {
      TType op = current().type;
      advance();

      auto right = parse_bitwise_or_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_bitwise_or_expr()
{
   auto left = parse_bitwise_xor_expr();

   while (is(TType::bitwise_or))
   {
      TType op = current().type;
      advance();

      auto right = parse_bitwise_xor_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_bitwise_xor_expr()
{
   auto left = parse_bitwise_and_expr();

   while (is(TType::bitwise_xor))
   {
      TType op = current().type;
      advance();

      auto right = parse_bitwise_and_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_bitwise_and_expr()
{
   auto left = parse_equality_expr();

   while (is(TType::bitwise_and))
   {
      TType op = current().type;
      advance();

      auto right = parse_equality_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_equality_expr()
{
   auto left = parse_relational_expr();

   while (is(TType::equals_equals) || is(TType::not_equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_relational_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_relational_expr()
{
   auto left = parse_shift_expr();

   while (is(TType::smaller) || is(TType::smaller_equals) || is(TType::bigger) || is(TType::bigger_equals))
   {
      TType op = current().type;
      advance();

      auto right = parse_shift_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_shift_expr()
{
   auto left = parse_additive_expr();

   while (is(TType::shift_left) || is(TType::shift_right))
   {
      TType op = current().type;
      advance();

      auto right = parse_additive_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_additive_expr()
{
   auto left = parse_multiplicative_expr();

   while (is(TType::plus) || is(TType::minus))
   {
      TType op = current().type;
      advance();

      auto right = parse_multiplicative_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_multiplicative_expr()
{
   auto left = parse_exponentiation_expr();

   while (is(TType::star) || is(TType::slash) || is(TType::percent))
   {
      TType op = current().type;
      advance();

      auto right = parse_exponentiation_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_exponentiation_expr()
{
   auto left = parse_unary_expr();

   while (is(TType::star_star))
   {
      TType op = current().type;
      advance();

      auto right = parse_exponentiation_expr();
      left = std::make_unique<BinaryExpr>(op, left, right);
   }
   return left;
}

Stmt Parser::parse_unary_expr()
{
   if (is(TType::minus) || is(TType::plus) || is(TType::logical_not) || is(TType::bitwise_not) || is(TType::bitwise_and) || is(TType::star) || is(TType::plus_plus) || is(TType::minus_minus))
   {
      TType op = tokens.at(index).type;
      advance();

      auto expr = parse_primary_expr();
      return std::make_unique<UnaryExpr>(op, expr);
   }
   return parse_primary_expr();
}

Stmt Parser::parse_increment_suffix()
{
   auto value = parse_primary_expr();

   if (is(TType::plus_plus) || is(TType::minus_minus))
   {
      TType op = tokens.at(index).type;
      advance();
      value = std::make_unique<UnaryExpr>(static_cast<TType>((int)op + 1), value);
   }
   return value;
}

Stmt Parser::parse_primary_expr()
{
   if (is(TType::identifier))
   {
      std::string identifier = current().lexeme;
      advance();
      return std::make_unique<Identifier>(identifier);
   }
   else if (is(TType::integer))
   {
      long long number = 0;

      try
      { number = std::stoll(current().lexeme); }
      catch (...)
      { this->catcher.insert(err::could_not_convert_number); }

      advance();
      return std::make_unique<IntegralLiteral>(number);
   }
   else if (is(TType::real))
   {
      long double number = 0.0;

      try
      { number = std::stold(current().lexeme); }
      catch (...)
      { this->catcher.insert(err::could_not_convert_number); }

      advance();
      return std::make_unique<RealLiteral>(number);
   }
   else if (is(TType::string))
   {
      std::string string = current().lexeme;
      advance();
      return std::make_unique<StringLiteral>(string);
   }
   else if (is(TType::character))
   {
      char ch = (current().lexeme.size() == 0 ? char{} : current().lexeme.at(0));
      advance();
      return std::make_unique<CharLiteral>(ch);
   }
   else if (is(TType::l_paren))
   {
      advance();
      auto value = parse_ternary_expr();

      if (!is(TType::r_paren))
         this->catcher.insert(err::mismatched_parentheses);
      advance();
      return value;
   }
   else
   {
      this->catcher.insert(err::expected_primary_expression);
      advance();
      return std::make_unique<NullLiteral>();
   }
}

void Parser::advance()
{
   if (this->index + 1 < this->tokens.size())
      ++this->index;
}

bool Parser::is(TType type) const
{
   return this->tokens.at(this->index).type == type;
}

Token& Parser::current()
{
   return this->tokens.at(this->index);
}
