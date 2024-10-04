#include <gtest/gtest.h>
#include "lexer.h"

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
    auto actual = tokenize(" \t   ");
    std::vector<Token> expected = {
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, identifiers)
{
    auto actual = tokenize("abc var_1 var2");
    std::vector<Token> expected = {
        Token{Token::Type::Identifier, "abc", 1, 1},
        Token{Token::Type::Identifier, "var_1", 1, 5},
        Token{Token::Type::Identifier, "var2", 1, 11},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, integers)
{
    auto actual = tokenize("1 123");
    std::vector<Token> expected = {
        Token{Token::Type::Integer, "1", 1, 1},
        Token{Token::Type::Integer, "123", 1, 3},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, real_numbers)
{
    auto actual = tokenize("3.1415 -3.1E12 .1e12 2E-12");
    std::vector<Token> expected = {
        Token{Token::Type::Real, "3.1415", 1, 1},
        Token{Token::Type::Real, "-3.1E12", 1, 8},
        Token{Token::Type::Real, ".1e12", 1, 16},
        Token{Token::Type::Real, "2E-12", 1, 22},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, strings)
{
    auto actual = tokenize("\"Hello world\"");
    std::vector<Token> expected = {
        Token{Token::Type::String, "Hello world", 1, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, string_with_escape_sequence)
{
    auto actual = tokenize(R"("Hello \"world\"")");
    std::vector<Token> expected = {
        Token{Token::Type::String, "Hello \"world\"", 1, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, multiple_lines)
{
    auto actual = tokenize("test\nlol\n123");
    std::vector<Token> expected = {
        Token{Token::Type::Identifier, "test", 1, 1},
        Token{Token::Type::Identifier, "lol", 2, 1},
        Token{Token::Type::Integer, "123", 3, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, comments)
{
    auto actual = tokenize("test // this is a comment\nlol");
    std::vector<Token> expected = {
        Token{Token::Type::Identifier, "test", 1, 1},
        Token{Token::Type::Identifier, "lol", 2, 1},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}

TEST(lexer_tests, multi_line_comments) {
    auto actual = tokenize("test /*\nthis is a comment\n*/lol");
    std::vector<Token> expected = {
        Token{Token::Type::Identifier, "test", 1, 1},
        Token{Token::Type::Identifier, "lol", 3, 3},
        Token{Token::Type::EndOfFile},
    };
    EXPECT_EQ(actual, expected);
}
