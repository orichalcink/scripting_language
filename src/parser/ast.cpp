#include "parser/ast.hpp"
#include <iostream>

AssignmentExpr::AssignmentExpr(TType op, Stmt& left, Stmt& right)
   : op(op), left(std::move(left)), right(std::move(right)) {}

ExprType AssignmentExpr::type() const
{
   return ExprType::assignment;
}

void AssignmentExpr::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Assignment Expression:\n";
   this->left->print(indentation + 2);
   std::cout << std::string(indentation, ' ') << "Operator: " << token_to_string(this->op) << "\n";
   this->right->print(indentation + 2);
}

TernaryExpr::TernaryExpr(Stmt& expr, Stmt& left, Stmt& right)
   : expr(std::move(expr)), left(std::move(left)), right(std::move(right)) {}

ExprType TernaryExpr::type() const
{
   return ExprType::ternary;
}

void TernaryExpr::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Ternary Expression:\n";
   this->expr->print(indentation + 2);
   std::cout << std::string(indentation, ' ') << "Operator: question\n";
   this->left->print(indentation + 2);
   std::cout << std::string(indentation, ' ') << "Operator: colon\n";
   this->right->print(indentation + 2);
}

BinaryExpr::BinaryExpr(TType op, Stmt& left, Stmt& right)
   : op(op), left(std::move(left)), right(std::move(right)) {}

ExprType BinaryExpr::type() const
{
   return ExprType::binary;
}

void BinaryExpr::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Binary Expression:\n";
   this->left->print(indentation + 2);
   std::cout << std::string(indentation, ' ') << "Operator: " << token_to_string(this->op) << "\n";
   this->right->print(indentation + 2);
}

UnaryExpr::UnaryExpr(TType op, Stmt& value)
   : op(op), value(std::move(value)) {}

ExprType UnaryExpr::type() const
{
   return ExprType::unary;
}

void UnaryExpr::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Unary Expression:\n";
   std::cout << std::string(indentation, ' ') << "Operator: " << token_to_string(this->op) << "\n";
   this->value->print(indentation + 2);
}

ExprType NullLiteral::type() const
{
   return ExprType::null;
}

void NullLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Null\n";
}

Identifier::Identifier(const std::string& identifier)
   : identifier(identifier) {}

ExprType Identifier::type() const
{
   return ExprType::identifier;
}

void Identifier::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "[" << identifier << "]\n";
}

RealLiteral::RealLiteral(long double number)
   : number(number) {}

ExprType RealLiteral::type() const
{
   return ExprType::real;
}

void RealLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation + 1, ' ') << this->number << "\n";
}

IntegralLiteral::IntegralLiteral(long long number)
   : number(number) {}

ExprType IntegralLiteral::type() const
{
   return ExprType::integer;
}

void IntegralLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation + 1, ' ') << this->number << "\n";
}

StringLiteral::StringLiteral(const std::string& string)
   : string(string) {}

ExprType StringLiteral::type() const
{
   return ExprType::string;
}

void StringLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "\"" << this->string << "\"\n";
}

CharLiteral::CharLiteral(char ch)
   : ch(ch) {}

ExprType CharLiteral::type() const
{
   return ExprType::character;
}

void CharLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "'" << this->ch << "'\n";
}

void Program::print() const
{
   for (const auto& stmt : this->statements)
      stmt->print(0);
}
