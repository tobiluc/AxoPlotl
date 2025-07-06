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
        ASSIGN,
        COMMA, LPAREN, RPAREN, LBOXBRACKET, RBOXBRACKET,
        PLUS, MINUS, TIMES, DIV, POW, INDEX,
        LOR, LAND, NOT,
        EQ, NEQ, LEQ, GEQ, LESS, GREATER
    };

    Token::Type TYPE;
    std::string LEXEME;

    static const Token UNKNOWN;
    static const Token END;
};

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "Token(" << (int)(token.TYPE) << ", \"" << token.LEXEME << "\")";
    return os;
}

std::vector<Token> tokenize(const char* text);

}

#endif // TOKENS_H
