#pragma once

#include "lexer.h"
#include "runtime.h"

#include <optional>
#include <stdexcept>
#include <vector>

struct ASTNode
{
    enum class Type {
        Invalid = 0,
        SingleNode,
        VarDeclaration,
        BinaryExpression,
        UnaryExpression,
        ParenthesizedExpression,
        FunctionDeclaration,
        ScopeBlock,
        IfStatement,
        WhileStatement,
        ArrayAccess,
        FieldAccess,
        FunctionCall,
        Constructor,
    } type;
    virtual ~ASTNode() = default;
    virtual bool operator==(const ASTNode &other) const = 0;
    virtual void compile(OLRuntime::Program &program) const
    {
        throw std::runtime_error("Unimplemented method!");
    }
    bool operator!=(const ASTNode &other) const { return !(*this == other); }
};

struct SingleNode final : public ASTNode
{
    Token token;
    explicit SingleNode(Token);
    void compile(OLRuntime::Program &program) const override;
    bool operator==(const ASTNode &other) const override;
};

struct VarDeclaration final : public ASTNode
{
    Token name;
    explicit VarDeclaration(Token name);
    bool operator==(const ASTNode &other) const override;
};

struct BinaryExpression final : public ASTNode
{
    ASTNode *left;
    ASTNode *right;
    Token op;
    BinaryExpression(ASTNode *left, ASTNode *right, Token op);
    ~BinaryExpression() override;
    void compile(OLRuntime::Program &program) const override;
    bool operator==(const ASTNode &other) const override;
};

struct ParenthesizedExpression final : public ASTNode
{
    ASTNode *expression;
    explicit ParenthesizedExpression(ASTNode *expression);
    ~ParenthesizedExpression() override;
    bool operator==(const ASTNode &other) const override;
};

struct UnaryExpression final : public ASTNode
{
    Token op;
    ASTNode *operand;
    UnaryExpression(Token op, ASTNode *operand);
    ~UnaryExpression() override;
    bool operator==(const ASTNode &other) const override;
};

struct FunctionDeclaration final : public ASTNode
{
    Token name;
    std::vector<ASTNode *> args;
    ASTNode *body;
    FunctionDeclaration(Token name, std::vector<ASTNode *> args, ASTNode *body);
    ~FunctionDeclaration() override;
    bool operator==(const ASTNode &other) const override;
};

struct FunctionCall final : public ASTNode
{
    ASTNode *name;
    std::vector<ASTNode *> args;
    FunctionCall(ASTNode *name, std::vector<ASTNode *> args);
    ~FunctionCall() override;
    bool operator==(const ASTNode &other) const override;
};

struct ScopeBlock final : public ASTNode
{
    std::vector<ASTNode *> statements;
    explicit ScopeBlock(std::vector<ASTNode *> statements);
    ~ScopeBlock() override;
    bool operator==(const ASTNode &other) const override;
};

struct IfStatement final : public ASTNode
{
    ASTNode *condition;
    ASTNode *body;
    std::optional<ASTNode *> else_body;
    IfStatement(ASTNode *condition, ASTNode *body, ASTNode *else_body);
    IfStatement(ASTNode *condition, ASTNode *body);
    ~IfStatement() override;
    bool operator==(const ASTNode &other) const override;
};

struct WhileStatement final : public ASTNode
{
    ASTNode *condition;
    ASTNode *body;
    WhileStatement(ASTNode *condition, ASTNode *body);
    ~WhileStatement() override;
    bool operator==(const ASTNode &other) const override;
};

struct ArrayAccess final : public ASTNode
{
    ASTNode *array;
    ASTNode *index;
    ArrayAccess(ASTNode *array, ASTNode *index);
    ~ArrayAccess() override;
    bool operator==(const ASTNode &other) const override;
};

struct FieldAccess final : public ASTNode
{
    ASTNode *record;
    ASTNode *field;
    FieldAccess(ASTNode *record, ASTNode *field);
    ~FieldAccess() override;
    bool operator==(const ASTNode &other) const override;
};

struct Constructor final : public ASTNode
{
    ASTNode *record;
    explicit Constructor(ASTNode *record);
    ~Constructor() override;
    bool operator==(const ASTNode &other) const override;
};

bool operator==(const std::vector<ASTNode *> &left, const std::vector<ASTNode *> &right);

void destroy_ast(const std::vector<ASTNode *> &ast);