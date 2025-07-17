#include "tokens.h"

namespace AxoPlotl::Parsing
{

const Token Token::UNKNOWN = Token{.type = Token::Type::UNKNOWN, .lexeme = ""};
const Token Token::END = Token{.type = Token::Type::END, .lexeme = ""};

inline bool isBinaryOperator(const Token& token) {
    return token.type == Token::Type::PLUS ||
           token.type == Token::Type::MINUS ||
           token.type == Token::Type::TIMES ||
           token.type == Token::Type::DIV ||
           token.type == Token::Type::LAND ||
           token.type == Token::Type::LOR ||
           token.type == Token::Type::EQ ||
           token.type == Token::Type::NEQ ||
           token.type == Token::Type::LEQ ||
           token.type == Token::Type::LESS ||
           token.type == Token::Type::GEQ ||
           token.type == Token::Type::GREATER ||
           token.type == Token::Type::POW;
}

inline bool isUnaryOperator(const Token& token) {
    return token.type == Token::Type::NOT ||
           token.type == Token::Type::UNARY_PLUS ||
           token.type == Token::Type::UNARY_MINUS;
}

std::vector<Token> tokenize(const char* text)
{
    std::vector<Token> tokens;
    const char* p = text;
    bool unary = false;

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
        // Single symbols
        case ',': tokens.push_back({Token::Type::COMMA, ","}); break;
        case '(': tokens.push_back({Token::Type::LPAREN, "("}); break;
        case ')': tokens.push_back({Token::Type::RPAREN, ")"}); break;
        case '*': tokens.push_back({Token::Type::TIMES, "*"}); break;
        case '/': tokens.push_back({Token::Type::DIV, "/"}); break;
        case '^': tokens.push_back({Token::Type::POW, "^"}); break;

        // Can be unary or binary
        case '+':
        case '-':
            // Check if we have a unary operator (after an operator)
            if (!tokens.empty()) {
                const auto& prev = tokens.back();
                unary = prev.type == Token::Type::LPAREN ||
                        prev.type == Token::Type::COMMA ||
                        isUnaryOperator(prev) || isBinaryOperator(prev);
            } else {unary = true;} // first token is unary
            if (*p == '+') {tokens.push_back({unary? Token::Type::UNARY_PLUS : Token::Type::PLUS, "+"}); break;}
            if (*p == '-') {tokens.push_back({unary? Token::Type::UNARY_MINUS : Token::Type::MINUS, "-"}); break;}


        // Tokens consisting of more than one symbol
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
