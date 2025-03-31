#include "parser/ast.hpp"
#include <iostream>

VarDeclaration::VarDeclaration(Stmt& ttype, const std::string& identifier, Stmt& body)
   : ttype(std::move(ttype)), identifier(identifier), body(std::move(body)) {}

StmtType VarDeclaration::type() const
{
   return StmtType::var_decl;
}

void VarDeclaration::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Variable Declaration:\n";
   this->ttype->print(indentation + 2);
   std::cout << std::string(indentation, ' ') << "Identifier: [" << this->identifier << "]\n";
   this->body->print(indentation + 2);
}

TypeExpr::TypeExpr(bool con, bool mut, bool automatic, const std::string& ttype)
   : con(con), mut(mut), automatic(automatic), ttype(ttype) {}

StmtType TypeExpr::type() const
{
   return StmtType::type;
}

void TypeExpr::print(size_t indentation) const
{
   std::cout << std::string(indentation + 1, ' ') << (this->mut ? "mut " : "");
   std::cout << (this->con ? "con " : "") << (this->automatic ? "var" : this->ttype) << "\n";
}

AssignmentExpr::AssignmentExpr(TType op, Stmt& left, Stmt& right)
   : op(op), left(std::move(left)), right(std::move(right)) {}

StmtType AssignmentExpr::type() const
{
   return StmtType::assignment;
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

StmtType TernaryExpr::type() const
{
   return StmtType::ternary;
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

StmtType BinaryExpr::type() const
{
   return StmtType::binary;
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

StmtType UnaryExpr::type() const
{
   return StmtType::unary;
}

void UnaryExpr::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Unary Expression:\n";
   std::cout << std::string(indentation, ' ') << "Operator: " << token_to_string(this->op) << "\n";
   this->value->print(indentation + 2);
}

StmtType NullLiteral::type() const
{
   return StmtType::null;
}

void NullLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "Null\n";
}

Identifier::Identifier(const std::string& identifier)
   : identifier(identifier) {}

StmtType Identifier::type() const
{
   return StmtType::identifier;
}

void Identifier::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "[" << identifier << "]\n";
}

RealLiteral::RealLiteral(long double number)
   : number(number) {}

StmtType RealLiteral::type() const
{
   return StmtType::real;
}

void RealLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation + 1, ' ') << this->number << "\n";
}

IntegralLiteral::IntegralLiteral(long long number)
   : number(number) {}

StmtType IntegralLiteral::type() const
{
   return StmtType::integer;
}

void IntegralLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation + 1, ' ') << this->number << "\n";
}

StringLiteral::StringLiteral(const std::string& string)
   : string(string) {}

StmtType StringLiteral::type() const
{
   return StmtType::string;
}

void StringLiteral::print(size_t indentation) const
{
   std::cout << std::string(indentation, ' ') << "\"" << this->string << "\"\n";
}

CharLiteral::CharLiteral(char ch)
   : ch(ch) {}

StmtType CharLiteral::type() const
{
   return StmtType::character;
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
