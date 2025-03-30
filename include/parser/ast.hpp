#ifndef AST_HPP
#define AST_HPP

#include "lexer/tokens.hpp"
#include <memory>
#include <vector>

enum class ExprType : std::int8_t
{ assignment, ternary, binary, unary, null, identifier, real, integer, string, character };

struct Statement
{
   virtual ~Statement() = default;
   virtual void print(size_t indentation) const = 0;
};

struct Expr : public Statement
{
   virtual ~Expr() = default;
   virtual ExprType type() const = 0;
};

using Stmt = std::unique_ptr<Statement>;

struct AssignmentExpr : public Expr
{
   TType op;
   Stmt left;
   Stmt right;

   AssignmentExpr(TType op, Stmt& left, Stmt& right);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct TernaryExpr : public Expr
{
   Stmt expr;
   Stmt left;
   Stmt right;

   TernaryExpr(Stmt& expr, Stmt& left, Stmt& right);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct BinaryExpr : public Expr
{
   TType op;
   Stmt left;
   Stmt right;

   BinaryExpr(TType op, Stmt& left, Stmt& right);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct UnaryExpr : public Expr
{
   TType op;
   Stmt value;

   UnaryExpr(TType op, Stmt& value);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct Identifier : public Expr
{
   std::string identifier;

   Identifier(const std::string& identifier);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct NullLiteral : public Expr
{
   NullLiteral() = default;
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct RealLiteral : public Expr
{
   long double number;

   RealLiteral(long double number);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct IntegralLiteral : public Expr
{
   long long number;

   IntegralLiteral(long long number);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct StringLiteral : public Expr
{
   std::string string;

   StringLiteral(const std::string& string);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct CharLiteral : public Expr
{
   char ch;

   CharLiteral(char ch);
   ExprType type() const override;
   void print(size_t indentation) const override;
};

struct Program
{
   std::vector<Stmt> statements;
   void print() const;
};

#endif // AST_HPP
