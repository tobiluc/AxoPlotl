#include "parsing.h"
#include <format>
#include <strstream>

namespace AxPl::Parsing
{

std::unique_ptr<ASTNode> Parser::parse()
{
    return parseExpression();
}

std::unique_ptr<ASTNode> Parser::parseExpression()
{
    return parseAssignment();
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    if (peekIs(0, Token::Type::IDENTIFIER) && peekIs(1, Token::Type::ASSIGN))
    {
        const auto& left = consume(Token::Type::IDENTIFIER);
        consume(Token::Type::ASSIGN);
        auto right = parseAssignment();
        return std::make_unique<AssignOpNode>(left.LEXEME, std::move(right));
    } else {
        return parseLogicOr();
    }
}

std::unique_ptr<ASTNode> Parser::parseLogicOr()
{
    auto lor = parseLogicAnd();
    while (peekIs(0, Token::Type::LOR))
    {
        const auto& token = advance();
        auto land = parseLogicAnd();
        lor = std::make_unique<BinaryOpNode>(token.TYPE, std::move(lor), std::move(land));
    }
    return lor;
}

std::unique_ptr<ASTNode> Parser::parseLogicAnd()
{
    auto land = parseEquality();
    while (peekIs(0, Token::Type::LAND))
    {
        const auto& token = advance();
        auto eq = parseEquality();
        land = std::make_unique<BinaryOpNode>(token.TYPE, std::move(land), std::move(eq));
    }
    return land;
}

std::unique_ptr<ASTNode> Parser::parseEquality()
{
    auto eq = parseComparison();
    while (peek(0).TYPE == Token::Type::NEQ || peek(0).TYPE == Token::Type::EQ)
    {
        const auto& token = advance();
        auto comp = parseComparison();
        eq = std::make_unique<BinaryOpNode>(token.TYPE, std::move(eq), std::move(comp));
    }
    return eq;
}

std::unique_ptr<ASTNode> Parser::parseComparison()
{
    auto comp = parseTerm();
    while (
        peekIs(0, Token::Type::GREATER) ||
        peekIs(0, Token::Type::GEQ) ||
        peekIs(0, Token::Type::LESS) ||
        peekIs(0, Token::Type::LEQ))
    {
        const auto& token = advance();
        auto term = parseTerm();
        comp = std::make_unique<BinaryOpNode>(token.TYPE, std::move(comp), std::move(term));
    }
    return comp;
}

std::unique_ptr<ASTNode> Parser::parseTerm()
{
    auto term = parseFactor();
    while (peekIs(0, Token::Type::MINUS) || peekIs(0, Token::Type::PLUS))
    {
        const auto& token = advance();
        auto factor = parseFactor();
        term = std::make_unique<BinaryOpNode>(token.TYPE, std::move(term), std::move(factor));
    }
    return term;
}

std::unique_ptr<ASTNode> Parser::parseFactor()
{
    auto factor = parseExponent();
    while (peekIs(0, Token::Type::TIMES) || peekIs(0, Token::Type::DIV))
    {
        const auto& token = advance();
        auto exp = parseExponent();
        factor = std::make_unique<BinaryOpNode>(token.TYPE, std::move(factor), std::move(exp));
    }
    return factor;
}

std::unique_ptr<ASTNode> Parser::parseExponent()
{
    auto exp = parseUnary();
    while (peekIs(0, Token::Type::POW))
    {
        const auto& token = advance();
        auto unary = parseUnary();
        exp = std::make_unique<BinaryOpNode>(token.TYPE, std::move(unary), std::move(exp));
    }
    return exp;
}

std::unique_ptr<ASTNode> Parser::parseUnary()
{
    Token token;

    if (peekIs(0, Token::Type::NOT) || peekIs(0, Token::Type::MINUS))
    {
        token = advance();
        return std::make_unique<UnaryOpNode>(token.TYPE, parseUnary());
    }

    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    std::unique_ptr<ASTNode> primary = nullptr;
    Token::Type type = peek(0).TYPE;

    if (type == Token::Type::INT ||
        type == Token::Type::FLOAT
    )
    {
        // Integer or Floating Point Number
        primary = std::make_unique<ScalarNode>(std::stod(peek(0).LEXEME));
    }
    else if (type == Token::Type::IDENTIFIER)
    {
        // Identifier followed by "(" indicates function call
        if (peekIs(1, Token::Type::LPAREN))
        {
            primary = parseCall();
        }
        else
        {
            // Identifier (Variable)
            primary = std::make_unique<VariableNode>(advance().LEXEME);
        }
    }
    else if (type == Token::Type::LBOXBRACKET)
    {
        primary = parseList();
    }
    else if (type == Token::Type::LPAREN)
    {
        consume(Token::Type::LPAREN);
        primary = parseExpression();
        consume(Token::Type::RPAREN);
    }
    else if (type == Token::Type::END)
    {
        return std::make_unique<EmptyNode>();
    }
    else
    {
        std::strstream ss;
        ss << "Unexpected Token " << peek(0) << " in Primary Expression";
        throw std::runtime_error(ss.str());
    }

    // Parse Indexing on List, Identifier or Expression
    if (type == Token::Type::IDENTIFIER ||
        type == Token::Type::LBOXBRACKET ||
        type == Token::Type::LPAREN)
    {
        while (match(Token::Type::LBOXBRACKET))
        {
            primary = std::make_unique<BinaryOpNode>(Token::Type::INDEX, std::move(primary), parseExpression());
            consume(Token::Type::RBOXBRACKET);
        }
    }

    return primary;
}

std::unique_ptr<ASTNode> Parser::parseCall()
{
    // var idToken = consume(Token.Type.ID);
    // consume(Token.Type.LPAREN);
    // var args = (peek() == Token.Type.RPAREN)? new ArrayList<Expr>() : parseExprList();
    // consume(Token.Type.RPAREN);
    // return new FuncCallExpr(context, idToken.LEXEME, args.toArray(new Expr[]{}));
}

std::unique_ptr<ASTNode> Parser::parseList()
{
    // Start with "["
    consume(Token::Type::LBOXBRACKET);

    std::vector<std::unique_ptr<ASTNode>> exprs;

    // Check if next token is "]" -> Empty List
    if (match(Token::Type::RBOXBRACKET))
    {
        return std::make_unique<ListNode>(exprs);
    }

    // Parse at least one Expression in the list, separated by ","
    exprs.push_back(parseExpression());
    while (match(Token::Type::COMMA))
    {
        exprs.push_back(parseExpression());
    }

    // End of List should be "]"
    consume(Token::Type::RBOXBRACKET);

    return std::make_unique<ListNode>(exprs);
}

}
