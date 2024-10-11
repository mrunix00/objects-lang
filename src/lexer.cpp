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
        || c == '-' || c == '*' || c == '/'
        || c == ';' || c == '[' || c == ']'
        || c == '.';
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
    if (str == ";") return Token::Type::Semicolon;
    if (str == ",") return Token::Type::Comma;
    if (str == "=") return Token::Type::Equals;
    if (str == "+") return Token::Type::Plus;
    if (str == "-") return Token::Type::Minus;
    if (str == "*") return Token::Type::Asterisk;
    if (str == "/") return Token::Type::Slash;
    if (str == "var") return Token::Type::Var;
    if (str == "if") return Token::Type::If;
    if (str == "else") return Token::Type::Else;
    if (str == "while") return Token::Type::While;
    if (str == "function") return Token::Type::Function;
    return Token::Type::Identifier;
}

Token Lexer::next()
{
    Token token;

    if (position >= source.size())
        return {Token::Type::EndOfFile};

    // skip whitespaces
    for (; isspace(source[position]) && position < source.size(); position++) {
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

    char current_char = source[position];

    // Handle '===' and '=='
    if (current_char == '=') {
        if (source[position + 1] == '=') {
            if (source[position + 2] == '=') {
                token.type = Token::Type::StrictEquality;
                token.value = "===";
                position += 3;
                column += 3;
            }
            else {
                token.type = Token::Type::LooseEquality;
                token.value = "==";
                position += 2;
                column += 2;
            }
        }
        else {
            token.type = Token::Type::Equals;
            token.value = "=";
            position++;
            column++;
        }
        return token;
    }

    // Handle single-character tokens like '('
#define TOK(tok, c) case c: token.type = Token::Type::tok; token.value = c; position++; column++; return token
    switch (current_char) {
        TOK(LeftParenthesis, '(');
        TOK(RightParenthesis, ')');
        TOK(LeftBracket, '[');
        TOK(RightBracket, ']');
        TOK(LeftBrace, '{');
        TOK(RightBrace, '}');
        case '.':
            if (isdigit(source[position + 1]))
                break;
            token.type = Token::Type::Dot;
            token.value = '.';
            position++;
            column++;
            return token;
        default:
            break;
    }

    if (source[position] == '"')
        return read_string();

    bool seen_exponent = false;
    while (true) {
        if (position >= source.size())
            break;

        token.value += source[position];

        if (is_integer(token.value) || is_real(token.value))
            token.type = Token::Type::Number;
        else
            token.type = classify_word(token.value);

        // handle exponents
        if (isdigit(source[position - 1]) &&
            (source[position] == 'E' || source[position] == 'e') && !seen_exponent) {
            seen_exponent = true;
            token.value += source[++position]; // Add 'E' or 'e'
            column++;

            if (source[position + 1] == '+' || source[position + 1] == '-') {
                token.value += source[++position]; // Add sign
            }
        }

        column++;
        position++;

        // an ugly workaround for real numbers
        if (token.type == Token::Type::Number &&
            source[position] == '.')
            continue;
        if (is_separator(source[position]) &&
            !(seen_exponent && (source[position] == '+' || source[position] == '-')))
            break;
    }

    last_token = token;
    return token;
}

Token Lexer::peek()
{
    auto o_line = this->line;
    auto o_column = this->column;
    auto o_position = this->position;
    auto old = current();
    auto token = next();
    this->line = o_line;
    this->column = o_column;
    this->position = o_position;
    this->last_token = old;
    return token;
}

#define ESCAPE_SEQ(CHAR, ESC) case CHAR: token.value += ESC; break
inline void Lexer::handle_escape_sequence(Token &token)
{
    position++; // skip '\'
    switch (source[position]) {
        ESCAPE_SEQ('n', '\n');
        ESCAPE_SEQ('r', '\r');
        ESCAPE_SEQ('t', '\t');
        ESCAPE_SEQ('"', '\"');
        ESCAPE_SEQ('\'', '\'');
        ESCAPE_SEQ('\\', '\\');
        default:
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

Token Lexer::current()
{
    return last_token;
}
