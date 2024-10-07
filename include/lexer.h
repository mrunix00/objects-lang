#pragma once

#include <string>

struct Token
{
    enum class Type
    {
        EndOfFile = 0,
        Identifier,
        Number,
        String,
        Plus,
        Minus,
        Asterisk,
        Slash,
        Equals,
        Var,
        If,
        Else,
        While,
        LeftParenthesis,
        RightParenthesis,
        Semicolon,
        LeftBrace,
        RightBrace,
        Function,
        Comma,
    } type;

    std::string value;
    size_t line = 0;
    size_t column = 0;

    inline bool operator==(const Token &other) const
    {
        return type == other.type &&
            value == other.value &&
            line == other.line &&
            column == other.column;
    }
};

class Lexer
{
    std::string source;
    size_t position;
    size_t line;
    size_t column;

    Token read_string();
    void handle_escape_sequence(Token &);

public:
    explicit Lexer(std::string);

    Token next();
    Token peek();
};