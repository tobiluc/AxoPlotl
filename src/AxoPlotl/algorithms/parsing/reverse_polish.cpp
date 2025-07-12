#include "reverse_polish.h"

namespace AxoPlotl::Parsing
{

struct OpInfo {
    int precedence = -1;
    bool rightAssociative = false;
};

std::unordered_map<Token::Type, OpInfo> opTable = {
    {Token::Type::PLUS, {4, false}}, {Token::Type::MINUS, {4, false}},
    {Token::Type::TIMES, {5, false}}, {Token::Type::DIV, {5, false}},
    {Token::Type::POW, {6, true}},
    {Token::Type::LAND, {2, false}}, {Token::Type::LOR, {1, false}},
    {Token::Type::NOT, {7, true}},
    {Token::Type::EQ, {3, false}}, {Token::Type::NEQ, {3, false}}, {Token::Type::LESS, {3, false}},
    {Token::Type::GREATER, {3, false}}, {Token::Type::LEQ, {3, false}}, {Token::Type::GEQ, {3, false}}
};

inline int precedence(Token::Type type) {
    auto it = opTable.find(type);
    return it != opTable.end() ? it->second.precedence : -1;
};

inline bool isRightAssoc(Token::Type type) {
    auto it = opTable.find(type);
    return it != opTable.end() && it->second.rightAssociative;
};

void reversePolish(const std::vector<Token>& tokens, RPN& rpn)
{
    // rpn stores a std::vector<Token> tokens or whatever else is required by an evaluator

    std::vector<Token> output;
    std::vector<Token> opStack;
    std::vector<uint> nargStack;

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        #define NEXTTOKEN ((i+1 < tokens.size())? tokens[i+1] : Token::UNKNOWN)

        const Token& token = tokens[i];
        switch (token.type)
        {
        case Token::Type::INT:
        case Token::Type::FLOAT:
            output.push_back(token);
            break;

        case Token::Type::IDENTIFIER:
            if (NEXTTOKEN.type == Token::Type::LPAREN) {
                opStack.push_back(token); // Function Name
            } else {
                output.push_back(token); // Variable
            }
            break;

        case Token::Type::COMMA:
            // Pop until left parenthesis
            while (!opStack.empty() && opStack.back().type != Token::Type::LPAREN) {
                output.push_back(opStack.back());
                opStack.pop_back();
            }
            // Increment arg count if tracking
            if (!nargStack.empty()) {++nargStack.back();}
            break;

        case Token::Type::LPAREN:
            // If function name is at the top of opStack, this is a function call
            opStack.push_back(token);
            if (!opStack.empty() && opStack.size() >= 2 &&
                opStack[opStack.size() - 2].type == Token::Type::IDENTIFIER) {

                    // Start counting args
                    if (NEXTTOKEN.type == Token::Type::RPAREN) {nargStack.push_back(0);} // function has no args
                    else {nargStack.push_back(1);}

            }
            break;

        case Token::Type::RPAREN:
            // Pop until left parenthesis
            while (!opStack.empty() && opStack.back().type != Token::Type::LPAREN) {
                output.push_back(opStack.back());
                opStack.pop_back();
            }
            if (opStack.empty()) {
                std::cerr << "Mismatched parentheses" << std::endl;
                return;
            }
            opStack.pop_back(); // Pop the LPAREN

            // If function name is next on stack, pop it to output
            if (!opStack.empty() && opStack.back().type == Token::Type::IDENTIFIER) {
                Token func = opStack.back();
                opStack.pop_back();
                func.metadata = nargStack.back(); // Attach number of args to token
                output.push_back(func);
                nargStack.pop_back();
            }
            break;

        default:
            if (opTable.find(token.type) != opTable.end()) {
                while (!opStack.empty()) {
                    const Token& top = opStack.back();
                    if (opTable.find(top.type) == opTable.end()) break;

                    int prec1 = precedence(token.type);
                    int prec2 = precedence(top.type);
                    bool rightAssoc = isRightAssoc(token.type);

                    if ((rightAssoc && prec1 < prec2) || (!rightAssoc && prec1 <= prec2)) {
                        output.push_back(top);
                        opStack.pop_back();
                    } else {
                        break;
                    }
                }
                opStack.push_back(token);
            } else if (token.type != Token::Type::END) {
                std::cerr << "Unexpected token: " << token.lexeme << std::endl;
                return;
            }
            break;
        }
    }

    // Pop remaining operators
    while (!opStack.empty()) {
        if (opStack.back().type == Token::Type::LPAREN || opStack.back().type == Token::Type::RPAREN) {
            std::cerr << "Mismatched parentheses in expression." << std::endl;
            return;
        }
        output.push_back(opStack.back());
        opStack.pop_back();
    }

    rpn.tokens = std::move(output);
}

double evaluate(const RPN& rpn, const Variables& vars, const Functions& funcs)
{
    std::vector<double> stack;

    for (const Token& token : rpn.tokens) {
        switch (token.type) {
        case Token::Type::INT:
        case Token::Type::FLOAT: {
            stack.push_back(std::stod(token.lexeme));
            break;
        }

        case Token::Type::IDENTIFIER: {
            // Look for variable
            auto it = vars.find(token.lexeme);
            if (it != vars.end()) {
                stack.push_back(it->second);
                break;
            }

            // Otherwise, treat it as a function
            auto fit = funcs.find(token.lexeme);
            if (fit == funcs.end()) {
                std::cerr << "Unknown variable or function: " << token.lexeme << std::endl;
                return 0.0;
            }

            // The number of arguments is stored in the token metadata
            std::vector<double> args;
            for (uint narg = 0; narg < token.metadata; ++narg) {
                args.push_back(stack.back());
                stack.pop_back();
            }
            double result = fit->second(args);
            stack.push_back(result);
            break;
        }

        case Token::Type::PLUS:
        case Token::Type::MINUS:
        case Token::Type::TIMES:
        case Token::Type::DIV:
        case Token::Type::POW:
        case Token::Type::EQ:
        case Token::Type::NEQ:
        case Token::Type::LESS:
        case Token::Type::LEQ:
        case Token::Type::GREATER:
        case Token::Type::GEQ:
        case Token::Type::LAND:
        case Token::Type::LOR: {
            if (stack.size() < 2) {
                std::cerr << "Insufficient operands for binary operator " << token.lexeme << std::endl;
                return 0.0;
            }

            double rhs = stack.back(); stack.pop_back();
            double lhs = stack.back(); stack.pop_back();
            double result;

            switch (token.type) {
            case Token::Type::PLUS: result = lhs + rhs; break;
            case Token::Type::MINUS: result = lhs - rhs; break;
            case Token::Type::TIMES: result = lhs * rhs; break;
            case Token::Type::DIV: result = lhs / rhs; break;
            case Token::Type::POW: result = std::pow(lhs, rhs); break;
            case Token::Type::EQ: result = lhs == rhs; break;
            case Token::Type::NEQ: result = lhs != rhs; break;
            case Token::Type::LESS: result = lhs < rhs; break;
            case Token::Type::LEQ: result = lhs <= rhs; break;
            case Token::Type::GREATER: result = lhs > rhs; break;
            case Token::Type::GEQ: result = lhs >= rhs; break;
            case Token::Type::LAND: result = (lhs != 0.0 && rhs != 0.0); break;
            case Token::Type::LOR:  result = (lhs != 0.0 || rhs != 0.0); break;
            default: std::cerr << "Unhandled binary operator" << std::endl; return 0.0;
            }

            stack.push_back(result);
            break;
        }

        case Token::Type::NOT: {
            if (stack.empty()) {
                std::cerr << "Insufficient operands for unary operator " << token.lexeme << std::endl;
                return 0.0;
            }
            double val = stack.back(); stack.pop_back();
            stack.push_back(!(bool)(val));
            break;
        }

        default:
            std::cerr << "Unexpected token in RPN: " << token.lexeme << std::endl;
            return 0.0;
        }
    }

    if (stack.size() != 1) {
        std::cerr << "Invalid RPN expression: stack size != 1" << std::endl;
        return 0.0;
    }

    return stack.back();
}

void registerCommons(Variables &vars, Functions& funcs)
{
    vars["pi"] = M_PI;
    vars["e"] = M_E;

    funcs["sin"] = [](const FunctionArgs& args) {
        if (args.size() != 1) {std::cerr << "sin expects 1 argument" << std::endl; return 0.0;}
        return std::sin(args[0]);
    };

    funcs["cos"] = [](const FunctionArgs& args) {
        if (args.size() != 1) {std::cerr << "cos expects 1 argument" << std::endl; return 0.0;}
        return std::cos(args[0]);
    };

    funcs["exp"] = [](const FunctionArgs& args) {
        if (args.size() != 1) {std::cerr << "exp expects 1 argument" << std::endl; return 0.0;}
        return std::exp(args[0]);
    };

    funcs["log"] = [](const FunctionArgs& args) {
        if (args.size() != 1) {std::cerr << "log expects 1 argument" << std::endl; return 0.0;}
        return std::log(args[0]);
    };
    funcs["ln"] = funcs["log"];

    funcs["sqrt"] = [](const FunctionArgs& args) {
        if (args.size() != 1) {std::cerr << "sqrt expects 1 argument" << std::endl; return 0.0;}
        return std::sqrt(args[0]);
    };

    funcs["max"] = [](const FunctionArgs& args) {
        if (args.size() == 0) {std::cerr << "max expects at least 1 argument" << std::endl; return 0.0;}
        double max = -std::numeric_limits<double>::infinity();
        for (const auto& arg : args) {if (arg > max) {max = arg;}}
        return max;
    };

    funcs["min"] = [](const FunctionArgs& args) {
        if (args.size() == 0) {std::cerr << "min expects at least 1 argument" << std::endl; return 0.0;}
        double min = std::numeric_limits<double>::infinity();
        for (const auto& arg : args) {if (arg < min) {min = arg;}}
        return min;
    };

    funcs["ifthenelse"] = [](const FunctionArgs& args) {
        if (args.size() != 3) {std::cerr << "ifthenelse expects 3 arguments" << std::endl; return 0.0;}
        return args[0]? args[1] : args[2];
    };
}

}
