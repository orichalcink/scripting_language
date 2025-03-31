#ifndef AST_HPP
#define AST_HPP

#include "lexer/tokens.hpp"
#include <memory>
#include <vector>

enum class StmtType : std::int8_t
{
   var_decl,
   type, assignment, ternary, binary, unary, null, identifier, real, integer, string, character
};

struct Statement
{
   virtual ~Statement() = default;
   virtual void print(size_t indentation) const = 0;
   virtual StmtType type() const = 0;
};

using Stmt = std::unique_ptr<Statement>;

struct VarDeclaration : public Statement
{
   Stmt ttype;
   std::string identifier;
   Stmt body;

   VarDeclaration(Stmt& ttype, const std::string& identifier, Stmt& body);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct TypeExpr : public Statement
{
   bool con;
   bool mut;
   bool automatic;
   std::string ttype;

   TypeExpr(bool con, bool mut, bool automatic, const std::string& ttype);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct AssignmentExpr : public Statement
{
   TType op;
   Stmt left;
   Stmt right;

   AssignmentExpr(TType op, Stmt& left, Stmt& right);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct TernaryExpr : public Statement
{
   Stmt expr;
   Stmt left;
   Stmt right;

   TernaryExpr(Stmt& expr, Stmt& left, Stmt& right);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct BinaryExpr : public Statement
{
   TType op;
   Stmt left;
   Stmt right;

   BinaryExpr(TType op, Stmt& left, Stmt& right);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct UnaryExpr : public Statement
{
   TType op;
   Stmt value;

   UnaryExpr(TType op, Stmt& value);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct Identifier : public Statement
{
   std::string identifier;

   Identifier(const std::string& identifier);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct NullLiteral : public Statement
{
   NullLiteral() = default;
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct RealLiteral : public Statement
{
   long double number;

   RealLiteral(long double number);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct IntegralLiteral : public Statement
{
   long long number;

   IntegralLiteral(long long number);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct StringLiteral : public Statement
{
   std::string string;

   StringLiteral(const std::string& string);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct CharLiteral : public Statement
{
   char ch;

   CharLiteral(char ch);
   StmtType type() const override;
   void print(size_t indentation) const override;
};

struct Program
{
   std::vector<Stmt> statements;
   void print() const;
};

#endif // AST_HPP
