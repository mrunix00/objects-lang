#include <gtest/gtest.h>
#include <vector>
#include "ast.h"
#include "parser.h"

TEST(parser_tests, parse_single_node)
{
    auto actual = parse("x");
    std::vector<ASTNode *> expected = {
        new SingleNode({Token::Type::Identifier, "x", 1, 1}),
    };
    EXPECT_EQ(expected, actual);
}

TEST(parser_tests, parse_var_assignment)
{
    auto actual = parse("x = 1");
    std::vector<ASTNode *> expected = {
        new BinaryExpression(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new SingleNode({Token::Type::Integer, "1", 1, 5}),
            {Token::Type::Equals, "=", 1, 3}
        ),
    };
    EXPECT_EQ(expected, actual);
}

TEST(parser_tests, parse_assignment_with_binary_expression)
{
    auto actual = parse("x = 1 + 2");
    std::vector<ASTNode *> expected = {
        new BinaryExpression(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new BinaryExpression(
                new SingleNode({Token::Type::Integer, "1", 1, 5}),
                new SingleNode({Token::Type::Integer, "2", 1, 9}),
                {Token::Type::Plus, "+", 1, 7}
            ),
            {Token::Type::Equals, "=", 1, 3}
        ),
    };
    EXPECT_EQ(expected, actual);
}
