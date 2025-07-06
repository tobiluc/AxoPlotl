#ifndef PARSING_H
#define PARSING_H

#include "AxoPlotl/parsing/tokens.h"
#include "AxoPlotl/parsing/tree.h"
#include <stack>

namespace AxoPlotl::Parsing
{

class Parser
{
private:
    uint index = 0;
    const std::vector<Token>& tokens;
    std::stack<Token::Type> brackets;

    static const std::array<std::pair<Token::Type, Token::Type>, 2> BRACKETS;

    inline const Token& peek(uint n = 0) const
    {
        if (index + n >= tokens.size()) return Token::END;
        return tokens[index + n];
    }

    inline bool peekIs(uint n, Token::Type type) const
    {
        return peek(n).TYPE == type;
    }

    inline const Token& advance()
    {
        if (isAtEnd()) return Token::END;
        const auto& token = tokens[index++];

        // Check for brackets mismatch
        for (uint i = 0; i < BRACKETS.size(); ++i) {
            if (token.TYPE == BRACKETS[i].first) //left bracket
            {
                brackets.push(token.TYPE);
                break;
            }
            else if (token.TYPE == BRACKETS[i].second) // right bracket
            {
                if (brackets.empty())
                {
                    // Brackets Mismatch (No opening bracket to closing bracket)
                    std::cerr << "Parser: Brackets Mismatch" << std::endl;
                }
                else
                {
                    const Token::Type last = brackets.top();
                    brackets.pop();
                    if (last != BRACKETS[i].first)
                    {
                        // Brackets Mismatch
                        std::cerr << "Parser: Brackets Mismatch" << std::endl;
                    }
                }
            }
        }

        return token;
    }

    /**
     * If the next token matches the expected type, it is consumed and true is returned.
     * Otherwise, the token is not cosumed and false is returned.
     **/
    inline bool match(Token::Type type)
    {
        if (isAtEnd()) return false;
        if (peekIs(0, type))
        {
            advance();
            return true;
        }
        return false;
    }

    inline const Token& consume(Token::Type expected)
    {
        if (!peekIs(0, expected))
        {
            printf("Unexpected Token. Expected %s but got %s\n", expected, peek(0).TYPE);
            return Token::UNKNOWN;
        }
        else
        {
            return advance();
        }
    }

    inline bool isAtEnd() const
    {
        return index >= tokens.size();
    }

public:
    Parser(const std::vector<Token>& tokens) :
        index(0),
        tokens(tokens)
    {}

    std::unique_ptr<ASTNode> parse();

private:

    /**
     * expression := assignment
     */
    std::unique_ptr<ASTNode> parseExpression();

    /**
     * assignment := (ID "=" expression) | (ID "(" (ID)+  ")" "=" expression) | logic_or
     */
    std::unique_ptr<ASTNode> parseAssignment();

    /**
     * logic_or := logic_and ( "or" logic_and )*
     */
    std::unique_ptr<ASTNode> parseLogicOr();

    /**
     * logic_and := equality ( "and" equality )*
     */
    std::unique_ptr<ASTNode> parseLogicAnd();

    /**
    * equality := comparison ( ( "!=" | "==" ) comparison )*
    **/
    std::unique_ptr<ASTNode> parseEquality();

    /**
    * comparison := term ( ( ">" | ">=" | "<" | "<=" ) term )*
    **/
    std::unique_ptr<ASTNode> parseComparison();

    /**
    * term := factor ( ( "-" | "+" ) factor )*
    **/
    std::unique_ptr<ASTNode> parseTerm();

    /**
    * factor := exponent ( ( "/" | "*" ) exponent )*
    **/
    std::unique_ptr<ASTNode> parseFactor();

    /**
     * exponent := unary (^ unary )*
     */
    std::unique_ptr<ASTNode> parseExponent();

    /**
     * unary := := ( "!" | "-" ) unary | primary
     **/
    std::unique_ptr<ASTNode> parseUnary();

    /**
     * primary := ("true" | "false" | NUMBER | STRING | "(" expression ")" | IDENTIFIER | function_call | array) ([expression])*
     **/
    std::unique_ptr<ASTNode> parsePrimary();

    std::unique_ptr<ASTNode> parseFunctionCall();

    /**
     * list := "[" "]" | "[" expression ("," expression)* "]"
     **/
    std::unique_ptr<ASTNode> parseList();

};


}

#endif // PARSING_H
