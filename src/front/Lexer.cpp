/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Diag.h"
#include <cctype>

namespace Front {
    Position::Position(const char *source) : source(source), index(-1), x(0), y(1), lineX(0), currentChar('\0') {
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
                this->position->lineX = this->position->x;
                this->position->x = 1;
            }
            this->position->Next();
        }

        /*
         * Determines whether the current character conforms to the character specified by the grammar
         * 判断当前字符是否符合文法规定的字符
         */
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
            case '=':
                this->MakeToken(TokenType::EQUAL, "=");
                this->position->Next();
                break;
            case ';':
                this->MakeToken(TokenType::SEMICOLON, ";");
                this->position->Next();
                break;
            default:
                if (this->IsDigit()) {
                    this->NumberToken();
                    break;
                }
                if (IsLetter()) {
                    this->IdentifierToken();
                    break;
                }
                DiagException(this->position, " current char '%c' is illegal");
        }
    }

    void Lexer::MakeToken(TokenType type, std::string_view value) {
        this->currentToken = std::make_shared<Token>(type, value);
    }

    void Lexer::NumberToken() {
        int start = this->position->index;

        while (this->IsDigit()) {
            this->position->Next();
        }

        this->MakeToken(TokenType::VT_NUMBER, this->position->source.substr(start, this->position->index - start));
    }

    void Lexer::IdentifierToken() {
        int start = this->position->index;

        while (this->IsLetterOrDigit()) {
            this->position->Next();
        }

        this->MakeToken(TokenType::IDENTIFIER, this->position->source.substr(start, this->position->index - start));
    }

    bool Lexer::IsLetter() {
        return (this->position->currentChar >= 'a' && this->position->currentChar <= 'z') || (this->position->currentChar >= 'A' && this->position->currentChar <= 'Z') || this->position->currentChar == '_';
    }

    bool Lexer::IsDigit() {
        return std::isdigit(this->position->currentChar);
    }

    bool Lexer::IsLetterOrDigit() {
        return this->IsLetter() || this->IsDigit();
    }

    void Lexer::ExpectToken(TokenType type) {
        if (this->currentToken->type == type) {
            this->Next();
        } else {
            DiagException(this->position, "%s expected", std::string(this->currentToken->value).data());
        }
    }

} // Front