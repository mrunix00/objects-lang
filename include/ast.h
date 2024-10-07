#pragma once

#include <optional>
#include <vector>
#include "lexer.h"

struct ASTNode
{
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

struct SingleNode: public ASTNode
{
    Token token;
    explicit SingleNode(Token);
    bool operator==(const ASTNode &other) const override;
};

struct VarDeclaration: public ASTNode
{
    Token name;
    explicit VarDeclaration(Token name);
    bool operator==(const ASTNode &other) const override;
};

struct BinaryExpression: public ASTNode
{
    ASTNode *left;
    ASTNode *right;
    Token op;
    BinaryExpression(ASTNode *left, ASTNode *right, Token op);
    ~BinaryExpression() override;
    bool operator==(const ASTNode &other) const override;
};

struct ParenthesizedExpression: public ASTNode
{
    ASTNode *expression;
    explicit ParenthesizedExpression(ASTNode *expression);
    ~ParenthesizedExpression() override;
    bool operator==(const ASTNode &other) const override;
};

struct UnaryExpression: public ASTNode
{
    Token op;
    ASTNode *operand;
    UnaryExpression(Token op, ASTNode *operand);
    ~UnaryExpression() override;
    bool operator==(const ASTNode &other) const override;
};

struct FunctionDeclaration: public ASTNode
{
    Token name;
    std::vector<ASTNode *> args;
    ASTNode *body;
    FunctionDeclaration(Token name, std::vector<ASTNode *> args, ASTNode *body);
    ~FunctionDeclaration() override;
    bool operator==(const ASTNode &other) const override;
};

struct ScopeBlock: public ASTNode
{
    std::vector<ASTNode *> statements;
    explicit ScopeBlock(std::vector<ASTNode *> statements);
    ~ScopeBlock() override;
    bool operator==(const ASTNode &other) const override;
};

bool operator==(const std::vector<ASTNode *> &left, const std::vector<ASTNode *> &right);

void destroy_ast(std::vector<ASTNode *> &ast);