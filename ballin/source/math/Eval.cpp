#include "math/Eval.hpp"

#include <algorithm>
#include <cassert>
#include <deque>
#include <stack>
#include <vector>
#include <print>

namespace ballin::math {

constexpr auto is_number(Token const& token) { return token.type == Token::Type::NUMBER; }
constexpr auto is_operator(Token const& token) { return token.type == Token::Type::OPERATOR; }
constexpr auto is_left_parenthesis(Token const& token) { return token.type == Token::Type::LPAREN; }
constexpr auto is_right_parenthesis(Token const& token) { return token.type == Token::Type::RPAREN; }

std::vector<Token> parse_expression(std::vector<Token> const& tokens)
{
    std::vector<Token> expression {};
    std::deque<Token> expressionOperators {};

    for (auto const& token : tokens)
    {
        switch (token.type)
        {
        case Token::Type::NUMBER: {
            expression.push_back(token);
            break;
        }
        case Token::Type::OPERATOR: {
            while (!expressionOperators.empty())
            {
                auto const hasLowerPrecedence = token.precedence < expressionOperators.front().precedence;
                auto const hasEqualPrecedence = token.precedence == expressionOperators.front().precedence;

                if (is_left_parenthesis(expressionOperators.front()) || !(hasLowerPrecedence || (hasEqualPrecedence && token.fixity == Token::Fixity::LEFT)))
                {
                    break;
                }

                expression.emplace_back(expressionOperators.front());
                expressionOperators.pop_front();
            }

            expressionOperators.push_front(token);
            break;
        }
        case Token::Type::LPAREN: {
            expressionOperators.push_front(token);
            break;
        }
        case Token::Type::RPAREN: {
            // FIXME: probably wont properly handle cases where you have r-parens. following each other.
            while (!expressionOperators.empty() && !is_left_parenthesis(expressionOperators.front()))
            {
                expression.emplace_back(expressionOperators.front());
                expressionOperators.pop_front();
            }

            assert(!expressionOperators.empty() && "MISMATCHING PARENTHESIS");

            expressionOperators.pop_front();
        }
        }
    }

    std::ranges::for_each(expressionOperators, [&] (auto const& expressionOperator) {
        expression.push_back(expressionOperator);
    });

    return expression;
}

float evaluate_expression(std::vector<Token> const& tokens)
{
    std::stack<float> expressionStack {};

    for (auto const& token : tokens)
    {
        switch (token.type)
        {
        case Token::Type::NUMBER: {
            float number {};
            std::stringstream { token.value } >> number;
            expressionStack.push(number);

            break;
        }
        case Token::Type::OPERATOR: {
            float lhs = expressionStack.top();
            expressionStack.pop();
            float rhs = expressionStack.top();
            expressionStack.pop();

            switch (token.value.front())
            {
            case '+': rhs += lhs; break;
            case '-': rhs -= lhs; break;
            case '*': rhs *= lhs; break;
            case '/': rhs /= lhs; break;
            }

            expressionStack.push(rhs);

            break;
        }
        case Token::Type::LPAREN: break;
        case Token::Type::RPAREN: break;

        default: assert(false && "UNRECOGNIZED TOKEN WAS REACHED");
        }
    }

    return expressionStack.top();
}

}

