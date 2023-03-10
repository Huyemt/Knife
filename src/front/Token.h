/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_TOKEN_H
#define KNIFE_TOKEN_H

#include <string_view>

namespace Front {
    /**
     * The Base of tokens
     * 单元类型
     */
    enum class TokenType {
        /**
         * Keyword
         * 关键字
         */
        IF,
        ELSE,
        WHILE,
        FOR,
        RETURN,

        /**
         * Identifier
         * 标识符
         */
        IDENTIFIER,

        /**
         * Delimiter
         * 界限符
         */
        //  (
        SLPAREN,
        //  )
        SRPAREN,
        //  [
        MLPAREN,
        //  ]
        MRPAREN,
        //  {
        BLPAREN,
        //  }
        BRPAREN,
        //  =
        ASSIGN,
        //  ,
        COMMA,
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
        STAR,
        //  /
        SLASH,
        //  ==
        EQUAL,
        //  !=
        NOT_EQUAL,
        //  >
        GREATER,
        //  >=
        GREATER_OR_EQUAL,
        //  <
        LESSER,
        //  =<
        //  <=
        LESSER_OR_EQUAL,


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
