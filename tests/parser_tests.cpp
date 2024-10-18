#include <gtest/gtest.h>
#include <vector>
#include "ast.h"
#include "parser.h"

#define BEGIN(SOURCE, ...) \
    auto actual = parse(SOURCE); \
    std::vector<ASTNode *> expected = {__VA_ARGS__}
#define END() \
    destroy_ast(actual); \
    destroy_ast(expected)

TEST(parser_tests, parse_single_node) {
    BEGIN(
        "x",
        new SingleNode({Token::Type::Identifier, "x", 1, 1}));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parse_var_assignment) {
    BEGIN(
        "x = 1",
        new BinaryExpression(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new SingleNode({Token::Type::Number, "1", 1, 5}),
            {Token::Type::Equals, "=", 1, 3}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parse_assignment_with_binary_expression) {
    BEGIN(
        "x = 1 + 2",
        new BinaryExpression(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new BinaryExpression(
                new SingleNode({Token::Type::Number, "1", 1, 5}),
                new SingleNode({Token::Type::Number, "2", 1, 9}),
                {Token::Type::Plus, "+", 1, 7}
            ),
            {Token::Type::Equals, "=", 1, 3}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parse_var_declarations) {
    BEGIN(
        "var x",
        new VarDeclaration(
            {Token::Type::Identifier, "x", 1, 5}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parse_var_declarations_with_assignment) {
    BEGIN(
        "var x = 1",
        new BinaryExpression(
            new VarDeclaration({Token::Type::Identifier, "x", 1, 5}),
            new SingleNode({Token::Type::Number, "1", 1, 9}),
            {Token::Type::Equals, "=", 1, 7}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parse_var_declarations_with_bin_expression) {
    BEGIN(
        "var x = 1 + 2",
        new BinaryExpression(
            new VarDeclaration({Token::Type::Identifier, "x", 1, 5}),
            new BinaryExpression(
                new SingleNode({Token::Type::Number, "1", 1, 9}),
                new SingleNode({Token::Type::Number, "2", 1, 13}),
                {Token::Type::Plus, "+", 1, 11}
            ),
            {Token::Type::Equals, "=", 1, 7}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, operator_precedence) {
    BEGIN(
        "1 + 2 * 3",
        new BinaryExpression(
            new SingleNode({Token::Type::Number, "1", 1, 1}),
            new BinaryExpression(
                new SingleNode({Token::Type::Number, "2", 1, 5}),
                new SingleNode({Token::Type::Number, "3", 1, 9}),
                {Token::Type::Asterisk, "*", 1, 7}
            ),
            {Token::Type::Plus, "+", 1, 3}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, operator_precedence_2) {
    BEGIN(
        "1 * 2 + 3",
        new BinaryExpression(
            new BinaryExpression(
                new SingleNode({Token::Type::Number, "1", 1, 1}),
                new SingleNode({Token::Type::Number, "2", 1, 5}),
                {Token::Type::Asterisk, "*", 1, 3}
            ),
            new SingleNode({Token::Type::Number, "3", 1, 9}),
            {Token::Type::Plus, "+", 1, 7}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, operator_precedence_3) {
    BEGIN(
        "x = 1 * 2 + 3 / 2",
        new BinaryExpression(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new BinaryExpression(
                new BinaryExpression(
                    new SingleNode({Token::Type::Number, "1", 1, 5}),
                    new SingleNode({Token::Type::Number, "2", 1, 9}),
                    {Token::Type::Asterisk, "*", 1, 7}
                ),
                new BinaryExpression(
                    new SingleNode({Token::Type::Number, "3", 1, 13}),
                    new SingleNode({Token::Type::Number, "2", 1, 17}),
                    {Token::Type::Slash, "/", 1, 15}
                ),
                {Token::Type::Plus, "+", 1, 11}
            ),
            {Token::Type::Equals, "=", 1, 3}
        )
    );
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, multiline_expression) {
    BEGIN(
        "1 + 2\n"
        "- 3\n"
        "2 + 3",
        new BinaryExpression(
            new SingleNode({Token::Type::Number, "1", 1, 1}),
            new BinaryExpression(
                new SingleNode({Token::Type::Number, "2", 1, 5}),
                new SingleNode({Token::Type::Number, "3", 2, 3}),
                {Token::Type::Minus, "-", 2, 1}
            ),
            {Token::Type::Plus, "+", 1, 3}
        ),
        new BinaryExpression(
            new SingleNode({Token::Type::Number, "2", 3, 1}),
            new SingleNode({Token::Type::Number, "3", 3, 5}),
            {Token::Type::Plus, "+", 3, 3}
        )
    );
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, parenthesis_precedence) {
    BEGIN(
        "(1 + 2) * 3",
        new BinaryExpression(
            new ParenthesizedExpression(
                new BinaryExpression(
                    new SingleNode({Token::Type::Number, "1", 1, 2}),
                    new SingleNode({Token::Type::Number, "2", 1, 6}),
                    {Token::Type::Plus, "+", 1, 4}
                )
            ),
            new SingleNode({Token::Type::Number, "3", 1, 11}),
            {Token::Type::Asterisk, "*", 1, 9}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, multiple_expressions) {
    BEGIN(
        "1 + 2; 3 + 4",
        new BinaryExpression(
            new SingleNode({Token::Type::Number, "1", 1, 1}),
            new SingleNode({Token::Type::Number, "2", 1, 5}),
            {Token::Type::Plus, "+", 1, 3}
        ),
        new BinaryExpression(
            new SingleNode({Token::Type::Number, "3", 1, 8}),
            new SingleNode({Token::Type::Number, "4", 1, 12}),
            {Token::Type::Plus, "+", 1, 10}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, multiple_expressions_2) {
    BEGIN(
        "1 + 2\n3 * 4",
        new BinaryExpression(
            new SingleNode({Token::Type::Number, "1", 1, 1}),
            new SingleNode({Token::Type::Number, "2", 1, 5}),
            {Token::Type::Plus, "+", 1, 3}
        ),
        new BinaryExpression(
            new SingleNode({Token::Type::Number, "3", 2, 1}),
            new SingleNode({Token::Type::Number, "4", 2, 5}),
            {Token::Type::Asterisk, "*", 2, 3}
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, function_declaration) {
    BEGIN(
        "function add(x, y) { x + y }",
        new FunctionDeclaration(
            {Token::Type::Identifier, "add", 1, 10},
            {
            new SingleNode({Token::Type::Identifier, "x", 1, 14}),
            new SingleNode({Token::Type::Identifier, "y", 1, 17}),
            },
            new ScopeBlock(
                {
                new BinaryExpression(
                    new SingleNode({Token::Type::Identifier, "x", 1, 22}),
                    new SingleNode({Token::Type::Identifier, "y", 1, 26}),
                    {Token::Type::Plus, "+", 1, 24}
                ),
                }
            )
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, function_call) {
    BEGIN(
        "add(1, 2)",
        new FunctionCall(
            new SingleNode({Token::Type::Identifier, "add", 1, 1}),
            {
            new SingleNode({Token::Type::Number, "1", 1, 5}),
            new SingleNode({Token::Type::Number, "2", 1, 8}),
            }
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, function_call_2) {
    BEGIN(
        "add(1 + 2, 3)",
        new FunctionCall(
            new SingleNode({Token::Type::Identifier, "add", 1, 1}),
            {
            new BinaryExpression(
                new SingleNode({Token::Type::Number, "1", 1, 5}),
                new SingleNode({Token::Type::Number, "2", 1, 9}),
                {Token::Type::Plus, "+", 1, 7}
            ),
            new SingleNode({Token::Type::Number, "3", 1, 12}),
            }
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, if_statement) {
    BEGIN(
        "if (x == 0) { x = 0 }",
        new IfStatement(
            new BinaryExpression(
                new SingleNode({Token::Type::Identifier, "x", 1, 5}),
                new SingleNode({Token::Type::Number, "0", 1, 10}),
                {Token::Type::LooseEquality, "==", 1, 7}
            ),
            new ScopeBlock(
                {
                new BinaryExpression(
                    new SingleNode({Token::Type::Identifier, "x", 1, 15}),
                    new SingleNode({Token::Type::Number, "0", 1, 19}),
                    {Token::Type::Equals, "=", 1, 17}
                ),
                }
            )
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, if_else_statement) {
    BEGIN(
        "if (x) { x = 0 } else { x = 1 }",
        new IfStatement(
            new SingleNode({Token::Type::Identifier, "x", 1, 5}),
            new ScopeBlock(
                {
                new BinaryExpression(
                    new SingleNode({Token::Type::Identifier, "x", 1, 10}),
                    new SingleNode({Token::Type::Number, "0", 1, 14}),
                    {Token::Type::Equals, "=", 1, 12}
                ),
                }
            ),
            new ScopeBlock(
                {
                new BinaryExpression(
                    new SingleNode({Token::Type::Identifier, "x", 1, 25}),
                    new SingleNode({Token::Type::Number, "1", 1, 29}),
                    {Token::Type::Equals, "=", 1, 27}
                ),
                }
            )
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, if_statement_no_scope_block) {
    BEGIN(
        "if (x) x = 0\n"
        "else x = 1",
        new IfStatement(
            new SingleNode({Token::Type::Identifier, "x", 1, 5}),
            new BinaryExpression(
                new SingleNode({Token::Type::Identifier, "x", 1, 8}),
                new SingleNode({Token::Type::Number, "0", 1, 12}),
                {Token::Type::Equals, "=", 1, 10}
            ),
            new BinaryExpression(
                new SingleNode({Token::Type::Identifier, "x", 2, 6}),
                new SingleNode({Token::Type::Number, "1", 2, 10}),
                {Token::Type::Equals, "=", 2, 8}
            )
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, while_statement) {
    BEGIN(
        "while (x) { x = 0 }",
        new WhileStatement(
            new SingleNode({Token::Type::Identifier, "x", 1, 8}),
            new ScopeBlock(
                {
                new BinaryExpression(
                    new SingleNode({Token::Type::Identifier, "x", 1, 13}),
                    new SingleNode({Token::Type::Number, "0", 1, 17}),
                    {Token::Type::Equals, "=", 1, 15}
                ),
                }
            )
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, array_accesser) {
    BEGIN(
        "x[0]",
        new ArrayAccess(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new SingleNode({Token::Type::Number, "0", 1, 3})
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, field_accessor) {
    BEGIN(
        "x.y",
        new FieldAccess(
            new SingleNode({Token::Type::Identifier, "x", 1, 1}),
            new SingleNode({Token::Type::Identifier, "y", 1, 3})
        ));
    EXPECT_EQ(expected, actual);
    END();
}

TEST(parser_tests, constructor) {
    BEGIN(
        "new Point(0, 0)",
        new Constructor(
            new FunctionCall(
                new SingleNode({Token::Type::Identifier, "Point", 1, 5}),
                {
                new SingleNode({Token::Type::Number, "0", 1, 11}),
                new SingleNode({Token::Type::Number, "0", 1, 14}),
                }
            )
        ));
    EXPECT_EQ(expected, actual);
    END();
}
