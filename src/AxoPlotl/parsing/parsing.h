#ifndef PARSING_H
#define PARSING_H

#include "AxoPlotl/parsing/tokens.h"
#include "AxoPlotl/parsing/tree.h"

namespace AxPl::Parsing
{

class Parser
{
private:
    uint index = 0;
    const std::vector<Token>& tokens;

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
        return tokens[index++];
    }

    inline bool match(Token::Type type) const
    {
        return index < tokens.size() && tokens[index].TYPE == type;
    }

    inline const Token& consume(Token::Type expected)
    {
        if (!match(expected))
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
     * assignment := IDENTIFIER "=" assignment | logic_or
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

    std::unique_ptr<ASTNode> parseCall();

    /**
     * list := "[" "]" | "[" expression ("," expression)* "]"
     **/
    std::unique_ptr<ASTNode> parseList();

};


}

#endif // PARSING_H
