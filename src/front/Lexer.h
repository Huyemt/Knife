/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_LEXER_H
#define KNIFE_LEXER_H

#include "Token.h"
#include <memory>

namespace Front {

    /**
     * Used to locate code
     * 用于定位代码
     */
    class Position {
    public:
        /**
         * Code
         * 代码
         */
        std::string_view source;

        /**
         * Reading index
         * 读取到的索引
         */
        int index;

        /**
         * Horizontal coordinate
         * 横坐标
         */
        int x;

        /**
         * Vertical coordinate
         * 纵坐标
         */
        int y;

        /**
         * Front-of-line coordinate
         * 行开头坐标
         */
         int lineX;

        /**
         * The character currently read
         * 当前读取到的字符
         */
        char currentChar;

        explicit Position(const char* source);
        void Next();
    };

    /**
     * Lexical analyzer
     * 词法分析器
     */
    class Lexer {
    public:
        std::shared_ptr<Token> currentToken;
        std::shared_ptr<Position> position;

        explicit Lexer(const char* code);
        /**
         * Read the next lexical unit
         * 读取下一个词法单元
         */
        void Next();

        /**
         * Expect the next lexical unit type, and throw an error if it does not match
         * 预期下一个词法单元类型，如果不符合，那么抛出错误
         * @param type
         */
        void ExpectToken(TokenType type);
    private:
        /**
         * Judge whether the current character is a letter (including the underscore `_`)
         * 判断当前字符是否为字母 (包括了下划线`_`)
         * @return bool
         */
        bool IsLetter();
        /**
         * Judge whether the current character is a decimal number
         * 判断当前字符是否为十进制数
         * @return bool
         */
        bool IsDigit();
        /**
         * Judge whether the current character is a decimal number or a letter
         * 判断当前字符是否为十进制数或字母
         * @return bool
         */
        bool IsLetterOrDigit();

        void MakeToken(TokenType type, std::string_view value);
        void NumberToken();
        void IdentifierToken();
    };

} // Front

#endif //KNIFE_LEXER_H
