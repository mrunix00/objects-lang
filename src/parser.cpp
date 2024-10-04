#include <cassert>
#include "parser.h"

/**
 * Warning! This piece of shit was written by ChatGPT
 * I'm not really sure how it works, but it works
 * https://chatgpt.com/share/6700157a-9608-800a-a66e-a4f534cebc12
 * https://chatgpt.com/share/67012033-288c-800a-8861-4ba5a7637004
 * TODO: try to understand this piece of shit and refactor it
 * Also CLion keeps telling me that this code leaks memory
 * TODO 2: check if this code has actual memory leaks and fix them
*/

enum Precedence
{
    LOWEST = 0,
    ASSIGNMENT,
    SUM,      // + and -
    PRODUCT,  // * and /
};

static Precedence get_precedence(const Token::Type type)
{
    switch (type) {
        case Token::Type::Equals:
            return Precedence::ASSIGNMENT;
        case Token::Type::Plus:
        case Token::Type::Minus:
            return Precedence::SUM;
        case Token::Type::Asterisk:
        case Token::Type::Slash:
            return Precedence::PRODUCT;
        default:
            return Precedence::LOWEST;
    }
}

static ASTNode *parse_primary(Lexer &lexer)
{
    Token token = lexer.next();

    if (token.type == Token::Type::Integer || token.type == Token::Type::Identifier) {
        return new SingleNode(token);
    }

    return nullptr;
}

static ASTNode *parse_binary_expression(Lexer &lexer, ASTNode *left, Precedence precedence)
{
    while (get_precedence(lexer.peek().type) > precedence) {
        Token op = lexer.next();
        Precedence next_precedence = get_precedence(op.type);

        ASTNode *right = parse_primary(lexer);

        if (get_precedence(lexer.peek().type) > next_precedence) {
            right = parse_binary_expression(lexer, right, Precedence(next_precedence + 1));
        }

        left = new BinaryExpression(left, right, op);
    }

    return left;
}

static ASTNode *parse_assignment(Lexer &lexer)
{
    ASTNode *left = parse_primary(lexer);

    if (lexer.peek().type == Token::Type::Equals) {
        Token op = lexer.next();  // consume '='
        ASTNode *right = parse_binary_expression(
            lexer,
            parse_primary(lexer),
            Precedence::ASSIGNMENT
        );
        return new BinaryExpression(left, right, op);
    }

    return left;
}

static ASTNode *parse_var_declaration(Lexer &lexer)
{
    Token varToken = lexer.next();
    assert(varToken.type == Token::Type::Var);

    Token name = lexer.next();
    assert(name.type == Token::Type::Identifier);

    if (lexer.peek().type == Token::Type::Equals) {
        lexer.next();
        ASTNode *value = parse_binary_expression(
            lexer,
            parse_primary(lexer),
            Precedence::ASSIGNMENT);
        return new VarDeclaration(name, value);
    }

    return new VarDeclaration(name);
}

static ASTNode *parse_expression(Lexer &lexer)
{
    ASTNode *node;
    switch (lexer.peek().type) {
        case Token::Type::Var:
            node = parse_var_declaration(lexer);
            break;
        default:
            node = parse_assignment(lexer);
            break;
    }
    return node;
}

std::vector<ASTNode *> parse(const std::string &source)
{
    Lexer lexer(source);
    std::vector<ASTNode *> nodes;

    while (lexer.peek().type != Token::Type::EndOfFile) {
        nodes.push_back(parse_expression(lexer));
    }

    return nodes;
}