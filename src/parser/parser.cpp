#include "parser/parser.hpp"
#include "errors/errors.hpp"

using namespace std::string_literals;

Parser::Parser(Catcher& catcher, std::vector<Token>& tokens)
   : catcher(catcher), tokens(tokens) {}

Program& Parser::parse()
{
   while (!is(TType::eof))
   {
      this->program.statements.push_back(std::move(parse_stmt()));

      if (!catcher.empty())
         return this->program;
   }
   return this->program;
}

Stmt Parser::parse_stmt()
{
   return parse_var_declaration();
}

Stmt Parser::parse_var_declaration()
{
   auto ttype = parse_type();

   if (ttype->type() != StmtType::type)
      return ttype;
   
   auto ident = parse_primary_expr();

   if (ident->type() != StmtType::identifier)
   {
      this->catcher.insert(err::expected_identifier_var_decl);
      return ttype;
   }

   if (is(TType::semicolon))
   {
      advance();

      auto* t = static_cast<TypeExpr*>(ttype.get());

      if (!t->mut)
      {
         this->catcher.insert(err::expected_var_body);
         return ttype;
      }

      if (t->automatic)
      {
         this->catcher.insert(err::auto_must_have_body);
         return ttype;
      }

      auto* i = static_cast<Identifier*>(ident.get());
      Stmt body = std::make_unique<NullLiteral>();
      return std::make_unique<VarDeclaration>(ttype, i->identifier, body);
   }
   else if (is(TType::equals))
   {
      advance();
      auto* i = static_cast<Identifier*>(ident.get());
      auto body = parse_var_declaration();

      if (!is(TType::semicolon))
      {
         this->catcher.insert(err::statement_semicolon);
         return ttype;
      }
      advance();
      return std::make_unique<VarDeclaration>(ttype, i->identifier, body);
   }
   else
   {
      this->catcher.insert(err::expected_equals_or_semicolon);
      return ttype;
   }
}

Stmt Parser::parse_type()
{
   if (current().lexeme != "mut"s && current().lexeme != "con"s && current().lexeme != "let"s && !is_type())
      return parse_compound_bitwise_expr();
   
   bool con = false;
   bool mut = false;
   bool automatic = false;
   std::string ttype = "";

   if (current().lexeme == "mut"s)
   {
      mut = true;
      advance();
   }
   else if (current().lexeme == "con"s)
   {
      con = true;
      advance();
   }

   if (current().lexeme == "let"s)
   {
      automatic = true;
   }
   else if (is_type())
   {
      ttype = current().lexeme;
   }
   else
   {
      this->catcher.insert(err::expected_type);
   }
   advance();
   return std::make_unique<TypeExpr>(con, mut, automatic, std::move(ttype));
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
      auto value = parse_stmt();

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

bool Parser::is_type() const
{
   const auto& t = this->tokens.at(this->index);
   return t.type == TType::keyword && (t.lexeme == "int"s || t.lexeme == "real"s || t.lexeme == "char"s || t.lexeme == "string"s || t.lexeme == "bool"s); 
}

Token& Parser::current()
{
   return this->tokens.at(this->index);
}
