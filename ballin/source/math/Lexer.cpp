#include "math/Lexer.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>

#include <print>

namespace ballin::math {

Lexer::Lexer(std::string_view const expression):
    expressionStream_m(expression.data())
{
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens {};

    std::ranges::for_each(std::istream_iterator<std::string>{ expressionStream_m }, std::default_sentinel_t {}, [&] (auto&& word) {
        Token token {};

        token.value = word;

        // FIXME: properly handle expressions written with parenthesis in the form ``(a * b)``
        if (std::ranges::all_of(token.value, [] (auto&& byte) { return std::isdigit(byte) || byte == '.'; }))
        {
            token.type       = Token::Type::NUMBER;
            token.precedence = Token::Precedence::NONE;
        }
        else switch (token.value.front())
        {
        case '+':
        case '-': {
            token.type       = Token::Type::OPERATOR;
            token.precedence = Token::Precedence::_3;
            break;
        }
        case '*':
        case '/': {
            token.type       = Token::Type::OPERATOR;
            token.precedence = Token::Precedence::_2;
            break;
        }
        case '(': {
            token.type       = Token::Type::LPAREN;
            token.precedence = Token::Precedence::_1;
            break;
        }
        case ')': {
            token.type       = Token::Type::RPAREN;
            token.precedence = Token::Precedence::_1;
            break;

        default: assert(false && "UNRECOGNIZED SYMBOL WAS REACHED");
        }
        }

        tokens.push_back(token);
    });

    return tokens;
}

}

