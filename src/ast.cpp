#include "ast.h"

#include <utility>
#include <cassert>

bool operator==(const std::vector<ASTNode *> &left, const std::vector<ASTNode *> &right)
{
    if (left.size() != right.size())
        return false;
    for (size_t i = 0; i < left.size(); ++i) {
        if (*left[i] != *right[i])
            return false;
    }
    return true;
}

void destroy_ast(std::vector<ASTNode *> &ast)
{
    for (auto &node: ast)
        delete node;
}

SingleNode::SingleNode(Token token)
    : token(std::move(token))
{ type = Type::SingleNode; }
bool SingleNode::operator==(const ASTNode &other) const
{
    return type == other.type &&
        token == dynamic_cast<const SingleNode &>(other).token;
}

VarDeclaration::VarDeclaration(Token name)
    : name(std::move(name)), initializer()
{ type = Type::VarDeclaration; }
VarDeclaration::VarDeclaration(Token name, ASTNode *initializer)
    : name(std::move(name)), initializer(initializer)
{ type = Type::VarDeclaration; }
VarDeclaration::~VarDeclaration()
{
    if (initializer.has_value())
        delete initializer.value();
}
bool VarDeclaration::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &decl = dynamic_cast<const VarDeclaration &>(other);
    if (initializer.has_value() != decl.initializer.has_value())
        return false;
    if (initializer.has_value() && *initializer.value() != *decl.initializer.value())
        return false;
    return name == decl.name;
}

BinaryExpression::BinaryExpression(ASTNode *left, ASTNode *right, Token op)
    : left(left), right(right), op(std::move(op))
{
    type = Type::BinaryExpression;
    assert(left != nullptr && right != nullptr);
}
BinaryExpression::~BinaryExpression()
{
    delete left;
    delete right;
}
bool BinaryExpression::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &expr = dynamic_cast<const BinaryExpression &>(other);
    return *left == *expr.left && *right == *expr.right && op == expr.op;
}

UnaryExpression::UnaryExpression(Token op, ASTNode *operand)
    : op(std::move(op)), operand(operand)
{
    type = Type::UnaryExpression;
    assert(operand != nullptr);
}
UnaryExpression::~UnaryExpression()
{ delete operand; }
bool UnaryExpression::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &expr = dynamic_cast<const UnaryExpression &>(other);
    return op == expr.op && operand == expr.operand;
}
