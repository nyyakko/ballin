#pragma once

#include "Lexer.hpp"

namespace ballin::math {

std::vector<Token> parse_expression(std::vector<Token> const& tokens);
float evaluate_expression(std::vector<Token> const& tokens);

}

