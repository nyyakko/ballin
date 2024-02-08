#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <sstream>

namespace ballin::math {

struct Token
{
    enum class Type
    {
        NUMBER, OPERATOR, LPAREN, RPAREN
    };

    enum class Precedence { NONE, _3, _2, _1 };
    enum class Fixity { LEFT, RIGHT };

    Type type;
    Precedence precedence;
    Fixity fixity;
    std::string value;
};

class Lexer
{
public:
    explicit Lexer(std::string_view const expression);

    std::vector<Token> tokenize();

private:
    std::stringstream expressionStream_m {};
};

}
