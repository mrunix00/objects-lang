#include <gtest/gtest.h>
#include <vector>
#include "ast.h"
#include "parser.h"

#define ARGS(...) __VA_ARGS__
#define BEGIN(SOURCE, EXPECTED) \
    auto actual = parse(SOURCE); \
    std::vector<ASTNode *> expected = {EXPECTED}
#define END() \
    destroy_ast(actual); \
    destroy_ast(expected)

TEST(parser_tests, parse_single_node)
{
    BEGIN("x", ARGS(
        new SingleNode({Token::Type::Identifier, "x", 1, 1}),
    ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parse_var_assignment)
{
    BEGIN("x = 1", ARGS(
        new BinaryExpression(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new SingleNode({Token::Type::Integer, "1", 1, 5}),
            {Token::Type::Equals, "=", 1, 3}
        ),
    ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parse_assignment_with_binary_expression)
{
    BEGIN("x = 1 + 2", ARGS(
        new BinaryExpression(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new BinaryExpression(
                new SingleNode({Token::Type::Integer, "1", 1, 5}),
                new SingleNode({Token::Type::Integer, "2", 1, 9}),
                {Token::Type::Plus, "+", 1, 7}
            ),
            {Token::Type::Equals, "=", 1, 3}
        ),
    ));
    EXPECT_EQ(expected, actual);
    END();
}
