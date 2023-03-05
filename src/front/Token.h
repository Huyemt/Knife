/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_TOKEN_H
#define KNIFE_TOKEN_H

#include <string_view>

namespace Front {
    /**
     * The type of tokens
     * 单元类型
     */
    enum class TokenType {
        IDENTIFIER,

        /**
         * Delimiter
         * 界限符
         */
        //  (
        SLPAREN,
        //  )
        SRPAREN,
        //  =
        EQUAL,
        //  ;
        SEMICOLON,
        //  \0
        EOF_,

        /**
         * Operator
         * 运算符
         */
        //  +
        PLUS,
        //  -
        MINUS,
        //  *
        MUL,
        //  /
        DIV,

        /**
         * Const
         * 常量类型
         */
        VT_NUMBER,

    };


    /**
     * 词法单元
     */
    class Token {
    public:
        TokenType type;
        std::string_view value;

        Token(TokenType tokenType, std::string_view value);
    };

} // Front

#endif //KNIFE_TOKEN_H
