#include <cassert>
#include <stdexcept>
#include "parser.h"

static ASTNode *read_expression(Lexer &lexer);

ASTNode *read_var_declaration(Lexer &lexer)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::Var);
    token = lexer.next();
    assert(token.type == Token::Type::Identifier);

    if (lexer.peek().type == Token::Type::Equals) {
        lexer.next();
        return new VarDeclaration(token, read_expression(lexer));
    }
    else {
        return new VarDeclaration(token);
    }
}

static ASTNode *read_expression(Lexer &lexer)
{
    std::vector<ASTNode *> node;

    while (true) {
        if (lexer.peek().type == Token::Type::EndOfFile)
            break;
        switch (lexer.peek().type) {
            case Token::Type::Var:
                return read_var_declaration(lexer);
            case Token::Type::Integer:
            case Token::Type::Real:
            case Token::Type::String:
            case Token::Type::Identifier:
                node.push_back(new SingleNode(lexer.next()));
                break;
            case Token::Type::Plus:
            case Token::Type::Minus:
            case Token::Type::Asterisk:
            case Token::Type::Slash:
            case Token::Type::Equals: {
                auto op = lexer.next();
                auto left = node.back();
                node.pop_back();
                auto right = read_expression(lexer);
                node.push_back(new BinaryExpression(left, right, op));
                break;
            }
            default:
                throw std::runtime_error(&"Unhandled token: "[(int) lexer.peek().type]);
        }
    }
    assert(node.size() == 1);
    return node.back();
}

std::vector<ASTNode *> parse(const std::string &source)
{
    Lexer lexer(source);
    std::vector<ASTNode *> nodes;

    while (lexer.peek().type != Token::Type::EndOfFile) {
        nodes.push_back(read_expression(lexer));
    }

    return nodes;
}