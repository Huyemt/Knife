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
         * The character currently read
         * 当前读取到的字符
         */
        char currentChar;

        Position(const char* source);
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

        Lexer(const char* code);
        /**
         * Read the next lexical unit
         * 读取下一个词法单元
         */
        void Next();
    private:
        void MakeToken(TokenType type, std::string_view value);
        void NumberToken();
    };

} // Front

#endif //KNIFE_LEXER_H
