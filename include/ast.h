#pragma once

#include <optional>
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
        UnaryExpression
    } type;
    virtual ~ASTNode() = default;
    virtual bool operator==(const ASTNode &other) const = 0;
};

class SingleNode: ASTNode
{
public:
    Token token;
    explicit SingleNode(Token);
    bool operator==(const ASTNode &other) const override;
};

class VarDeclaration: ASTNode
{
    std::string name;
    std::optional<ASTNode *> initializer;
public:
    explicit VarDeclaration(std::string name);
    VarDeclaration(std::string name, ASTNode *initializer);
    bool operator==(const ASTNode &other) const override;
};

class BinaryExpression: ASTNode
{
    ASTNode *left;
    ASTNode *right;
    Token op;
public:
    BinaryExpression(ASTNode *left, ASTNode *right, Token op);
    bool operator==(const ASTNode &other) const override;
};

class UnaryExpression: ASTNode
{
    Token op;
    ASTNode *operand;
public:
    UnaryExpression(Token op, ASTNode *operand);
    bool operator==(const ASTNode &other) const override;
};
