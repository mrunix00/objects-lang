#include "lexer.h"
#include <gtest/gtest.h>

std::vector<Token> tokenize(const std::string &input)
{
    Lexer lexer(input);
    std::vector<Token> tokens;
    while (true) {
        tokens.push_back(lexer.next());
        if (tokens.back().type == Token::Type::EndOfFile)
            break;
    }
    return tokens;
}

TEST(lexer_tests, skip_whitespace)
{
    const auto actual = tokenize(" \t   ");
    const std::vector<Token> expected = {
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, identifiers)
{
    const auto actual = tokenize("abc var_1 var2");
    const std::vector<Token> expected = {
        Token{Token::Type::Identifier, "abc", 1, 1},
        Token{Token::Type::Identifier, "var_1", 1, 5},
        Token{Token::Type::Identifier, "var2", 1, 11},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, integers)
{
    const auto actual = tokenize("1 123");
    const std::vector<Token> expected = {
        Token{Token::Type::Number, "1", 1, 1},
        Token{Token::Type::Number, "123", 1, 3},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, real_numbers)
{
    const auto actual = tokenize("3.1415 -3.1E12 .1e12 2E-12");
    const std::vector<Token> expected = {
        Token{Token::Type::Number, "3.1415", 1, 1},
        Token{Token::Type::Number, "-3.1E12", 1, 8},
        Token{Token::Type::Number, ".1e12", 1, 16},
        Token{Token::Type::Number, "2E-12", 1, 22},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, strings)
{
    const auto actual = tokenize("\"Hello world\"");
    const std::vector<Token> expected = {
        Token{Token::Type::String, "Hello world", 1, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, string_with_escape_sequence)
{
    const auto actual = tokenize(R"("Hello \"world\"")");
    const std::vector<Token> expected = {
        Token{Token::Type::String, "Hello \"world\"", 1, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, multiple_lines)
{
    const auto actual = tokenize("test\nlol\n123");
    const std::vector<Token> expected = {
        Token{Token::Type::Identifier, "test", 1, 1},
        Token{Token::Type::Identifier, "lol", 2, 1},
        Token{Token::Type::Number, "123", 3, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, comments)
{
    const auto actual = tokenize("test // this is a comment\nlol");
    const std::vector<Token> expected = {
        Token{Token::Type::Identifier, "test", 1, 1},
        Token{Token::Type::Identifier, "lol", 2, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, multi_line_comments)
{
    const auto actual = tokenize("test /*\nthis is a comment\n*/lol");
    const std::vector<Token> expected = {
        Token{Token::Type::Identifier, "test", 1, 1},
        Token{Token::Type::Identifier, "lol", 3, 3},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, operators)
{
    const auto actual = tokenize("+-*/= == ===");
    const std::vector<Token> expected = {
        Token{Token::Type::Plus, "+", 1, 1},
        Token{Token::Type::Minus, "-", 1, 2},
        Token{Token::Type::Asterisk, "*", 1, 3},
        Token{Token::Type::Slash, "/", 1, 4},
        Token{Token::Type::Equals, "=", 1, 5},
        Token{Token::Type::LooseEquality, "==", 1, 7},
        Token{Token::Type::StrictEquality, "===", 1, 10},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, keywords)
{
    const auto actual = tokenize(
        "var if else while function "
        "this return new true false null");
    const std::vector<Token> expected = {
        Token{Token::Type::Var, "var", 1, 1},
        Token{Token::Type::If, "if", 1, 5},
        Token{Token::Type::Else, "else", 1, 8},
        Token{Token::Type::While, "while", 1, 13},
        Token{Token::Type::Function, "function", 1, 19},
        Token{Token::Type::This, "this", 1, 28},
        Token{Token::Type::Return, "return", 1, 33},
        Token{Token::Type::New, "new", 1, 40},
        Token{Token::Type::True, "true", 1, 44},
        Token{Token::Type::False, "false", 1, 49},
        Token{Token::Type::Null, "null", 1, 55},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, punctuation)
{
    const auto actual = tokenize("();{},[].");
    const std::vector<Token> expected = {
        Token{Token::Type::LeftParenthesis, "(", 1, 1},
        Token{Token::Type::RightParenthesis, ")", 1, 2},
        Token{Token::Type::Semicolon, ";", 1, 3},
        Token{Token::Type::LeftBrace, "{", 1, 4},
        Token{Token::Type::RightBrace, "}", 1, 5},
        Token{Token::Type::Comma, ",", 1, 6},
        Token{Token::Type::LeftBracket, "[", 1, 7},
        Token{Token::Type::RightBracket, "]", 1, 8},
        Token{Token::Type::Dot, ".", 1, 9},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}
