#ifndef TOKENS_H
#define TOKENS_H

#include <iostream>
#include <vector>
#include <cctype>
#include <string>

namespace AxoPlotl::Parsing
{

struct Token
{

    enum class Type : unsigned char
    {
        UNKNOWN, END,
        INT, FLOAT, IDENTIFIER,
        COMMA, LPAREN, RPAREN,
        PLUS, MINUS, TIMES, DIV, POW,
        UNARY_PLUS, UNARY_MINUS,
        LOR, LAND, NOT,
        EQ, NEQ, LEQ, GEQ, LESS, GREATER
    };

    Token::Type type;
    std::string lexeme;
    u_int64_t metadata = 0;

    static const Token UNKNOWN;
    static const Token END;
};

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "Token(" << (int)(token.type) << ", \"" << token.lexeme << "\", " << token.metadata  <<")";
    return os;
}

std::vector<Token> tokenize(const char* text);

}

#endif // TOKENS_H
