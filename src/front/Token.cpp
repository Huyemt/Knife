/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Token.h"

namespace Front {
    Token::Token(TokenType tokenType, std::string_view value) {
        this->type = tokenType;
        this->value = value;
    }
} // Front