#include "lexer.h"

#include <utility>
#include <stdexcept>

Lexer::Lexer(std::string source)
    : source(std::move(source)),
      position(0),
      line(1),
      column(1)
{}

static inline bool is_separator(char c)
{
    return isspace(c) || c == ',' || c == '('
        || c == ')' || c == '{' || c == '}'
        || c == '\0' || c == '=' || c == '+'
        || c == '*' || c == '/';
}

static inline bool is_integer(const std::string &str)
{
    try {
        size_t idx;
        std::stoi(str, &idx);
        return idx == str.size();
    }
    catch (std::invalid_argument &e) {
        return false;
    }
}

static inline bool is_real(const std::string &str)
{
    try {
        size_t idx;
        std::stod(str, &idx);
        return idx == str.size();
    }
    catch (std::invalid_argument &e) {
        return false;
    }
}

static inline Token::Type classify_word(const std::string &str)
{
    if (str == "=") return Token::Type::Equals;
    if (str == "+") return Token::Type::Plus;
    if (str == "-") return Token::Type::Minus;
    if (str == "*") return Token::Type::Asterisk;
    if (str == "/") return Token::Type::Slash;
    return Token::Type::Identifier;
}

Token Lexer::next()
{
    Token token;

    if (position >= source.size())
        return {Token::Type::EndOfFile};

    // skip whitespaces
    for (; isspace(source[position]) && position < source.size(); position++) {
        // handle new line characters
        if (source[position] == '\n') {
            line++;
            column = 1;
        }
        else {
            column++;
        }
    }

    // skip one-line comments
    if (source[position] == '/' && source[position + 1] == '/') {
        position += 2;
        while (source[position] != '\n' && position < source.size()) {
            position++;
        }
        position++;
        column = 1;
        line++;
        return next();
    }

    // skip multi-line comments
    if (source[position] == '/' && source[position + 1] == '*') {
        position += 2;
        column += 2;
        for (; source[position] != '*' || source[position + 1] != '/'; position++) {
            if (source[position] == '\n') {
                line++;
                column = 1;
                continue;
            }
            column++;
        }
        position += 2;
        column += 2;
        return next();
    }

    if (position >= source.size())
        return {Token::Type::EndOfFile};

    token.column = column;
    token.line = line;

    if (source[position] == '"')
        return read_string();
    do {
        token.value += source[position];
        if (is_integer(token.value))
            token.type = Token::Type::Integer;
        else if (is_real(token.value))
            token.type = Token::Type::Real;
        else
            token.type = classify_word(token.value);
        column++;
        position++;
    }
    while (!is_separator(source[position]));

    return token;
}

Token Lexer::peek()
{
    auto o_line = this->line;
    auto o_column = this->column;
    auto o_position = this->position;
    auto token = next();
    this->line = o_line;
    this->column = o_column;
    this->position = o_position;
    return token;
}

inline void Lexer::handle_escape_sequence(Token &token)
{
    position++; // skip '\'
    switch (source[position]) {
        case 'n':
            token.value += '\n';
            break;
        case 'r':
            token.value += '\r';
            break;
        case 't':
            token.value += '\t';
            break;
        case '"':
            token.value += '"';
            break;
        case '\'':
            token.value += '\'';
            break;
        case '\\':
            token.value += '\\';
            break;
        default:
            token.value += source[position];
            break;
    }
    column++;
}

Token Lexer::read_string()
{
    Token token;
    token.type = Token::Type::String;
    token.line = line;
    token.column = column;
    position++; // skip "
    for (; source[position] != '"'; position++) {
        if (source[position] == '\\')
            handle_escape_sequence(token);
        else {
            token.value += source[position];
            column++;
        }
    }
    position++;
    column++;
    return token;
}
