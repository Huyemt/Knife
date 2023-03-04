/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Lexer.h"
#include <cctype>

namespace Front {
    Position::Position(const char *source) : source(source), index(-1), x(0), y(1), currentChar('\0') {
        /*
         * Read a character
         * 预读一个字符
         */
        this->Next();
    }

    void Position::Next() {
        this->index++;
        this->x++;

        this->currentChar = (this->index < this->source.length()) ? this->source[this->index] : '\0';
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    Lexer::Lexer(const char *code) {
        this->position = std::make_shared<Position>(code);
        /*
         * Read a token
         * 预读一个词法单元
         */
        this->Next();
    }

    void Lexer::Next() {
        /*
         * If the current character is blank, then read the next character
         * 如果当前是空白字符，那么读取下一个字符
         */
        while (isspace(this->position->currentChar)) {
            if (this->position->currentChar == '\n') {
                this->position->y++;
                this->position->x = 1;
            }
            this->position->Next();
        }

        switch (this->position->currentChar) {
            case '\0':
                this->MakeToken(TokenType::EOF_, "");
                break;
            case '+':
                this->MakeToken(TokenType::PLUS, "+");
                this->position->Next();
                break;
            case '-':
                this->MakeToken(TokenType::MINUS, "-");
                this->position->Next();
                break;
            case '*':
                this->MakeToken(TokenType::MUL, "*");
                this->position->Next();
                break;
            case '/':
                this->MakeToken(TokenType::DIV, "/");
                this->position->Next();
                break;
            case '(':
                this->MakeToken(TokenType::SLPAREN, "(");
                this->position->Next();
                break;
            case ')':
                this->MakeToken(TokenType::SRPAREN, ")");
                this->position->Next();
                break;
            default:
                if (isdigit(this->position->currentChar)) {
                    this->NumberToken();
                    break;
                }
                // throw
                exit(0);
        }
    }

    void Lexer::MakeToken(TokenType type, std::string_view value) {
        this->currentToken = std::make_shared<Token>(type, value);
    }

    void Lexer::NumberToken() {
        int start = this->position->index;

        while (isdigit(this->position->currentChar)) {
            this->position->Next();
        }

        this->MakeToken(TokenType::VT_NUMBER, this->position->source.substr(start, this->position->index - start));
    }

} // Front