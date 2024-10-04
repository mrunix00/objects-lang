#include "parser.h"

/**
 * Warning! This piece of shit was written by ChatGPT
 * I'm not really sure how it works, but it works
 * https://chatgpt.com/share/6700157a-9608-800a-a66e-a4f534cebc12
 * TODO: try to understand this piece of shit and refactor it
 * Also CLion keeps telling me that this code leaks memory
 * TODO 2: check if this code has actual memory leaks and fix them
*/

enum Precedence
{
    LOWEST = 0,
    ASSIGNMENT,
    SUM,
    PRODUCT,
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

static ASTNode *parse_binary_expression(Lexer &lexer, ASTNode *left, Precedence precedence)
{
    while (get_precedence(lexer.peek().type) > precedence) {
        auto op = lexer.next();
        Precedence next_precedence = get_precedence(op.type);

        ASTNode *right;
        right = new SingleNode(lexer.next());

        if (get_precedence(lexer.peek().type) > next_precedence) {
            right = parse_binary_expression(lexer, right, Precedence(next_precedence + 1));
        }

        left = new BinaryExpression(left, right, op);
    }

    return left;
}

ASTNode *parse_assignment(Lexer &lexer)
{
    auto left = new SingleNode(lexer.next());

    if (lexer.peek().type == Token::Type::Equals) {
        auto op = lexer.next();
        auto right = parse_binary_expression(
            lexer,
            new SingleNode(lexer.next()),
            Precedence::ASSIGNMENT
        );
        return new BinaryExpression(left, right, op);
    }

    return left;
}

std::vector<ASTNode *> parse(const std::string &source)
{
    Lexer lexer(source);
    std::vector<ASTNode *> nodes;

    while (lexer.peek().type != Token::Type::EndOfFile) {
        nodes.push_back(parse_assignment(lexer));
    }

    return nodes;
}
