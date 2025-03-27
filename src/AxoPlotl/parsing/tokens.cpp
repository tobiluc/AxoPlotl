#include "tokens.h"

namespace AxPl::Parsing
{

const Token Token::UNKNOWN = Token{.TYPE = Token::Type::UNKNOWN, .LEXEME = ""};
const Token Token::END = Token{.TYPE = Token::Type::END, .LEXEME = ""};

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

        // Ambigouities
        // TODO

        // Single-Character Tokens
        switch (*p)
        {
        case '=': tokens.push_back({Token::Type::ASSIGN, "="}); break;
        case ',': tokens.push_back({Token::Type::COMMA, ","}); break;
        case '(': tokens.push_back({Token::Type::LPAREN, "("}); break;
        case ')': tokens.push_back({Token::Type::RPAREN, ")"}); break;
        case '[': tokens.push_back({Token::Type::LBOXBRACKET, "["}); break;
        case ']': tokens.push_back({Token::Type::RBOXBRACKET, "]"}); break;
        case '+': tokens.push_back({Token::Type::PLUS, "+"}); break;
        case '-': tokens.push_back({Token::Type::MINUS, "-"}); break;
        case '*': tokens.push_back({Token::Type::TIMES, "*"}); break;
        case '/': tokens.push_back({Token::Type::DIV, "/"}); break;
        case '^': tokens.push_back({Token::Type::POW, "^"}); break;
        case '!': tokens.push_back({Token::Type::NOT, "!"}); break;
        default: tokens.push_back({Token::Type::UNKNOWN, std::string(1, *p)}); break;
        }

        // Advance Character
        ++p;
    }

    return tokens;
}

}
