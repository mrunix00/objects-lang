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
    if (node->type == ASTNode::Type::BinaryExpression) {
        auto op = dynamic_cast<const BinaryExpression *>(node)->op.type;
        return get_precedence(op);
    }
    else {
        return Precedence::Lowest;
    }
}

static ASTNode *read_expression(Lexer &lexer, std::vector<ASTNode *> &);

static ASTNode *read_var_declaration(Lexer &lexer, std::vector<ASTNode *> &nodes)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::Var);
    token = lexer.next();
    assert(token.type == Token::Type::Identifier);
    return new VarDeclaration(token);
}

static ASTNode *read_expression(Lexer &lexer, std::vector<ASTNode *> &nodes)
{
    while (true) {
        if (lexer.peek().type == Token::Type::EndOfFile)
            break;
        switch (lexer.peek().type) {
            case Token::Type::Var:
                return read_var_declaration(lexer, nodes);
            case Token::Type::Integer:
            case Token::Type::Real:
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
                if (left->type == ASTNode::Type::BinaryExpression) {
                    if (get_precedence(left) >= get_precedence(op.type)) {
                        return new BinaryExpression(left, right, op);
                    }
                    else {
                        auto l_bin = dynamic_cast<BinaryExpression *>(left);
                        l_bin->right = new BinaryExpression(l_bin->right, right, op);
                        return l_bin;
                    }
                }
                return new BinaryExpression(left, right, op);
            }
            default:
                throw std::runtime_error(&"Unhandled token: "[(int) lexer.peek().type]);
        }
    }
    assert(nodes.size() == 1);
    return nodes.back();
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