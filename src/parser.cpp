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
        case Token::Type::StrictEquality:
        case Token::Type::LooseEquality:
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

static bool is_expression_ended(const Token &current, const Token &next)
{
    if (next.type == Token::Type::Semicolon ||
        next.type == Token::Type::EndOfFile)
        return true;
    if (next.line > current.line)
        return true;
    return false;
}

static ASTNode *read_expression(Lexer &lexer, std::vector<ASTNode *> &nodes);
static ASTNode *read_expression(Lexer &lexer)
{
    std::vector<ASTNode *> nodes;
    while (!is_expression_ended(lexer.current(), lexer.peek()))
        nodes.push_back(read_expression(lexer, nodes));
    assert(nodes.size() == 1);
    return nodes.back();
}

static ASTNode *read_scope_block(Lexer &lexer)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::LeftBrace);
    std::vector<ASTNode *> statements;
    while (lexer.peek().type != Token::Type::RightBrace) {
        assert(lexer.peek().type != Token::Type::EndOfFile);
        statements.push_back(read_expression(lexer, statements));
    }
    token = lexer.next();
    assert(token.type == Token::Type::RightBrace);
    return new ScopeBlock(statements);
}

static ASTNode *read_func_declaration(Lexer &lexer)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::Function);

    // read function name
    token = lexer.next();
    assert(token.type == Token::Type::Identifier);
    auto name = token;

    // read function arguments
    token = lexer.next();
    assert(token.type == Token::Type::LeftParenthesis);
    std::vector<ASTNode *> args;
    while (true) {
        assert(lexer.peek().type != Token::Type::EndOfFile);
        args.push_back(read_expression(lexer, args));
        token = lexer.next();
        if (token.type == Token::Type::RightParenthesis)
            break;
        assert(token.type == Token::Type::Comma);
    }

    // read function body
    token = lexer.peek();
    assert(token.type == Token::Type::LeftBrace);
    auto body = read_scope_block(lexer);

    return new FunctionDeclaration(name, args, body);
}

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

static ASTNode *read_if_statement(Lexer &lexer)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::If);

    // read condition
    token = lexer.next();
    assert(token.type == Token::Type::LeftParenthesis);
    std::vector<ASTNode *> nodes;
    while (lexer.peek().type != Token::Type::RightParenthesis) {
        assert(lexer.peek().type != Token::Type::EndOfFile);
        nodes.push_back(read_expression(lexer, nodes));
    }
    token = lexer.next();
    assert(nodes.size() == 1);
    auto condition = nodes.back();
    assert(token.type == Token::Type::RightParenthesis);

    // read body
    token = lexer.peek();
    ASTNode *body = token.type == Token::Type::LeftBrace ?
                    read_scope_block(lexer) :
                    read_expression(lexer);

    // read else clause if found
    if (lexer.peek().type == Token::Type::Else) {
        lexer.next(); // consume 'else' token
        ASTNode *elseClause;
        if (lexer.peek().type == Token::Type::LeftBrace)
            elseClause = read_scope_block(lexer);
        else
            elseClause = read_expression(lexer);
        return new IfStatement(condition, body, elseClause);
    }

    return new IfStatement(condition, body);
}

static ASTNode *read_while_statement(Lexer &lexer)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::While);

    // read condition
    token = lexer.next();
    assert(token.type == Token::Type::LeftParenthesis);
    std::vector<ASTNode *> nodes;
    while (lexer.peek().type != Token::Type::RightParenthesis) {
        assert(lexer.peek().type != Token::Type::EndOfFile);
        nodes.push_back(read_expression(lexer, nodes));
    }
    token = lexer.next();
    assert(nodes.size() == 1);
    auto condition = nodes.back();
    assert(token.type == Token::Type::RightParenthesis);

    // read body
    token = lexer.peek();
    assert(token.type == Token::Type::LeftBrace);
    auto body = read_scope_block(lexer);

    return new WhileStatement(condition, body);
}

static ASTNode *read_array_access(Lexer &lexer, std::vector<ASTNode *> &nodes)
{
    auto token = lexer.next();
    assert(token.type == Token::Type::LeftBracket);

    // read array index
    std::vector<ASTNode *> index_nodes;
    while (lexer.peek().type != Token::Type::RightBracket) {
        assert(lexer.peek().type != Token::Type::EndOfFile);
        index_nodes.push_back(read_expression(lexer, index_nodes));
    }
    assert(index_nodes.size() == 1);

    token = lexer.next();
    assert(token.type == Token::Type::RightBracket);

    auto array = nodes.back();
    nodes.pop_back();
    return new ArrayAccess(array, index_nodes.back());
}

static ASTNode *read_function_call(Lexer &lexer)
{
    auto name = lexer.current();
    assert(name.type == Token::Type::Identifier);

    std::vector<ASTNode *> args;
    auto token = lexer.next();
    assert(token.type == Token::Type::LeftParenthesis);
    while (lexer.peek().type != Token::Type::RightParenthesis) {
        assert(lexer.peek().type != Token::Type::EndOfFile);
        args.push_back(read_expression(lexer, args));
        if (lexer.peek().type == Token::Type::Comma)
            lexer.next();
    }
    token = lexer.next();
    assert(token.type == Token::Type::RightParenthesis);

    return new FunctionCall(new SingleNode(name), args);
}

static ASTNode *read_expression(Lexer &lexer, std::vector<ASTNode *> &nodes)
{
    while (true) {
        if (lexer.peek().type == Token::Type::EndOfFile)
            break;
        if (lexer.peek().type == Token::Type::Semicolon) {
            lexer.next();
            return read_expression(lexer, nodes);
        }
        switch (lexer.peek().type) {
            case Token::Type::If:
                return read_if_statement(lexer);
            case Token::Type::While:
                return read_while_statement(lexer);
            case Token::Type::Function:
                return read_func_declaration(lexer);
            case Token::Type::Var:
                return read_var_declaration(lexer);
            case Token::Type::Number:
            case Token::Type::String:
                return new SingleNode(lexer.next());
            case Token::Type::Identifier: {
                auto id = lexer.next();
                if (lexer.peek().type == Token::Type::LeftParenthesis)
                    return read_function_call(lexer);
                return new SingleNode(id);
            }
            case Token::Type::Plus:
            case Token::Type::Minus:
            case Token::Type::Asterisk:
            case Token::Type::Slash:
            case Token::Type::Equals:
            case Token::Type::LooseEquality:
            case Token::Type::StrictEquality: {
                auto op = lexer.next();
                auto left = nodes.back();
                nodes.pop_back();
                auto right = read_expression(lexer, nodes);
                return handle_precedence(left, right, op);
            }
            case Token::Type::Dot: {
                auto token = lexer.next(); // consume '.'
                auto field = read_expression(lexer, nodes);
                auto record = nodes.back();
                nodes.pop_back();
                return new FieldAccess(record, field);
            }
            case Token::Type::LeftParenthesis:
                return read_parenthesized_expression(lexer);
            case Token::Type::LeftBracket:
                return read_array_access(lexer, nodes);
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
