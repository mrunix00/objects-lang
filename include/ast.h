#pragma once

#include <optional>
#include <vector>
#include "lexer.h"

class ASTNode
{
public:
    enum class Type
    {
        Invalid = 0,
        SingleNode,
        VarDeclaration,
        BinaryExpression,
        UnaryExpression,
        ParenthesizedExpression,
        FunctionDeclaration,
        ScopeBlock
    } type;
    virtual ~ASTNode() = default;
    virtual bool operator==(const ASTNode &other) const = 0;
    bool operator!=(const ASTNode &other) const
    { return !(*this == other); }
};

class SingleNode: public ASTNode
{
public:
    Token token;
    explicit SingleNode(Token);
    bool operator==(const ASTNode &other) const override;
};

class VarDeclaration: public ASTNode
{
    Token name;
public:
    explicit VarDeclaration(Token name);
    bool operator==(const ASTNode &other) const override;
};

class BinaryExpression: public ASTNode
{
public:
    ASTNode *left;
    ASTNode *right;
    Token op;
    BinaryExpression(ASTNode *left, ASTNode *right, Token op);
    ~BinaryExpression() override;
    bool operator==(const ASTNode &other) const override;
};

class ParenthesizedExpression: public ASTNode
{
public:
    ASTNode *expression;
    explicit ParenthesizedExpression(ASTNode *expression);
    ~ParenthesizedExpression() override;
    bool operator==(const ASTNode &other) const override;
};

class UnaryExpression: public ASTNode
{
    Token op;
    ASTNode *operand;
public:
    UnaryExpression(Token op, ASTNode *operand);
    ~UnaryExpression() override;
    bool operator==(const ASTNode &other) const override;
};

class FunctionDeclaration: public ASTNode
{
    Token name;
    std::vector<ASTNode *> args;
    ASTNode *body;
public:
    FunctionDeclaration(Token name, std::vector<ASTNode *> args, ASTNode *body);
    FunctionDeclaration(Token name, ASTNode *body);
    ~FunctionDeclaration() override;
    bool operator==(const ASTNode &other) const override;
};

class ScopeBlock: public ASTNode
{
    std::vector<ASTNode *> statements;
public:
    explicit ScopeBlock(std::vector<ASTNode *> statements);
    ~ScopeBlock() override;
    bool operator==(const ASTNode &other) const override;
};

bool operator==(const std::vector<ASTNode *> &left, const std::vector<ASTNode *> &right);

void destroy_ast(std::vector<ASTNode *> &ast);