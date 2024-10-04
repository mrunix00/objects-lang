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
        UnaryExpression
    } type;
    virtual ~ASTNode() = default;
    virtual bool operator==(const ASTNode &other) const = 0;
    bool operator!=(const ASTNode &other) const {
        return !(*this == other);
    }
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
    std::string name;
    std::optional<ASTNode *> initializer;
public:
    explicit VarDeclaration(std::string name);
    VarDeclaration(std::string name, ASTNode *initializer);
    ~VarDeclaration() override;
    bool operator==(const ASTNode &other) const override;
};

class BinaryExpression: public ASTNode
{
    ASTNode *left;
    ASTNode *right;
    Token op;
public:
    BinaryExpression(ASTNode *left, ASTNode *right, Token op);
    ~BinaryExpression() override;
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

bool operator==(const std::vector<ASTNode *> &left, const std::vector<ASTNode *> &right);

void destroy_ast(std::vector<ASTNode *> &ast);