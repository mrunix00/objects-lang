#include <cassert>
#include <stdexcept>
#include <iostream>
#include "parser.h"

enum class Precedence
{
    Lowest = 0,
    Equals,
    Sum,
    Product,
    Parenthesis,
};

static Precedence get_precedence(Token::Type type)
{
    switch (type) {
        case Token::Type::Equals:
            return Precedence::Equals;
        case Token::Type::Plus:
        case Token::Type::Minus:
            return Precedence::Sum;
        case Token::Type::Asterisk:
        case Token::Type::Slash:
            return Precedence::Product;
        default:
            return Precedence::Lowest;
    }
}

static Precedence get_precedence(const ASTNode *node)
{
    switch (node->type) {
        case ASTNode::Type::BinaryExpression:
            return get_precedence(dynamic_cast<const BinaryExpression *>(node)->op.type);
        case ASTNode::Type::ParenthesizedExpression:
            return Precedence::Parenthesis;
        default:
            return Precedence::Lowest;
    }
}

static ASTNode *read_var_declaration(Lexer &lexer)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::Var);
    token = lexer.next();
    assert(token.type == Token::Type::Identifier);
    return new VarDeclaration(token);
}

static ASTNode *handle_precedence(ASTNode *old, ASTNode *new_node, const Token &op)
{
    if (old->type != ASTNode::Type::BinaryExpression) {
        return new BinaryExpression(old, new_node, op);
    }
    auto old_node = dynamic_cast<BinaryExpression *>(old);
    if (get_precedence(op.type) >= get_precedence(old)) {
        auto right =
            old_node->right->type == ASTNode::Type::BinaryExpression ?
            handle_precedence(old_node->right, new_node, op) :
            new BinaryExpression(old_node->right, new_node, op);
        return new BinaryExpression(old_node->left, right, old_node->op);
    }
    else {
        return new BinaryExpression(old, new_node, op);
    }
}

static ASTNode *read_expression(Lexer &lexer, std::vector<ASTNode *> &nodes);

static ASTNode *read_parenthesized_expression(Lexer &lexer)
{
    std::vector<ASTNode *> nodes;
    auto token = lexer.next();
    assert(token.type == Token::Type::LeftParenthesis);
    while (lexer.peek().type != Token::Type::RightParenthesis) {
        assert(lexer.peek().type != Token::Type::EndOfFile);
        nodes.push_back(read_expression(lexer, nodes));
    }
    token = lexer.next();
    assert(token.type == Token::Type::RightParenthesis);
    assert(nodes.size() == 1);
    return new ParenthesizedExpression(nodes.back());
}

static ASTNode *read_expression(Lexer &lexer, std::vector<ASTNode *> &nodes)
{
    while (true) {
        if (lexer.peek().type == Token::Type::EndOfFile)
            break;
        switch (lexer.peek().type) {
            case Token::Type::Var:
                return read_var_declaration(lexer);
            case Token::Type::Number:
            case Token::Type::String:
            case Token::Type::Identifier:
                return new SingleNode(lexer.next());
            case Token::Type::Plus:
            case Token::Type::Minus:
            case Token::Type::Asterisk:
            case Token::Type::Slash:
            case Token::Type::Equals: {
                auto op = lexer.next();
                auto left = nodes.back();
                nodes.pop_back();
                auto right = read_expression(lexer, nodes);
                return handle_precedence(left, right, op);
            }
            case Token::Type::LeftParenthesis:
                return read_parenthesized_expression(lexer);
            default:
                throw std::runtime_error("Unhandled token: " +
                    std::to_string((int) lexer.peek().type));
        }
    }
    return nullptr;
}

std::vector<ASTNode *> parse(const std::string &source)
{
    Lexer lexer(source);
    std::vector<ASTNode *> nodes;

    while (lexer.peek().type != Token::Type::EndOfFile) {
        nodes.push_back(read_expression(lexer, nodes));
    }

    return nodes;
}