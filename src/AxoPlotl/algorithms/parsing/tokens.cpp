#include "tokens.h"

namespace AxoPlotl::Parsing
{

const Token Token::UNKNOWN = Token{.type = Token::Type::UNKNOWN, .lexeme = ""};
const Token Token::END = Token{.type = Token::Type::END, .lexeme = ""};

std::vector<Token> tokenize(const char* text)
{
    std::vector<Token> tokens;
    const char* p = text;

    while (*p)
    {
        // Skip Whitespace between Tokens
        if (std::isspace(*p))
        {
            ++p;
            continue;
        }

        // Numbers (Integers or Decimals)
        if (std::isdigit(*p))
        {
            const char* start = p;
            while (std::isdigit(*p)) ++p;

            // Dot (.) indicates Decimal
            if (*p == '.')
            {
                ++p;
                while (std::isdigit(*p)) ++p;
                tokens.push_back({Token::Type::FLOAT, std::string(start, p)});
            }
            else
            {
                tokens.push_back({Token::Type::INT, std::string(start, p)});
            }
            continue;
        }

        // Identifiers
        if (std::isalpha(*p) || *p == '_')
        {
            const char* start = p;
            while (std::isalnum(*p) || *p == '_') ++p;
            tokens.push_back({Token::Type::IDENTIFIER, std::string(start, p)});
            continue;
        }

        // Tokens
        switch (*p)
        {
        case ',': tokens.push_back({Token::Type::COMMA, ","}); break;
        case '(': tokens.push_back({Token::Type::LPAREN, "("}); break;
        case ')': tokens.push_back({Token::Type::RPAREN, ")"}); break;
        case '+': tokens.push_back({Token::Type::PLUS, "+"}); break;
        case '-': tokens.push_back({Token::Type::MINUS, "-"}); break;
        case '*': tokens.push_back({Token::Type::TIMES, "*"}); break;
        case '/': tokens.push_back({Token::Type::DIV, "/"}); break;
        case '^': tokens.push_back({Token::Type::POW, "^"}); break;
        case '!':
            if (*(p+1)=='=') {++p; tokens.push_back({Token::Type::NEQ, "!="});}
            else {tokens.push_back({Token::Type::NOT, "!"});}
            break;
        case '<':
            if (*(p+1)=='=') {++p; tokens.push_back({Token::Type::LEQ, "<="});}
            else {tokens.push_back({Token::Type::LESS, "<"});}
            break;
        case '>':
            if (*(p+1)=='=') {++p; tokens.push_back({Token::Type::GEQ, ">="});}
            else {tokens.push_back({Token::Type::GREATER, ">"});}
            break;
        case '=':
            if (*(p+1)=='=') {++p; tokens.push_back({Token::Type::EQ, "=="}); break;}
        case '|':
            if (*(p+1)=='|') {++p; tokens.push_back({Token::Type::LOR, "||"}); break;}
        case '&':
            if (*(p+1)=='&') {++p; tokens.push_back({Token::Type::LAND, "&&"}); break;}
        default: tokens.push_back({Token::Type::UNKNOWN, std::string(1, *p)}); break;
        }

        // Advance Character
        ++p;
    }

    //for (const auto& t : tokens) std::cout << t << std::endl;

    return tokens;
}

}
