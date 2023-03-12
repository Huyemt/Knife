/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Diag.h"
#include "Lexer.h"
#include <cctype>
#include <cassert>

namespace Front {
    Position::Position(const char *source) : Source(source), Index(-1), x(0), y(1), LineX(0), CurrentChar('\0') {
        /*
         * Read a character
         * 预读一个字符
         */
        this->Next();
    }

    void Position::Next() {
        this->Index++;
        this->x++;

        this->CurrentChar = (this->Index < this->Source.length()) ? this->Source[this->Index] : '\0';
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    Lexer::Lexer(const char *code) {
        this->position = std::make_shared<Position>(code);
        this->peekPosition = std::make_shared<Position>(code);
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
        while (isspace(this->position->CurrentChar)) {
            if (this->position->CurrentChar == '\n') {
                this->position->y++;
                this->position->LineX = this->position->Index + 1;
                this->position->x = 1;
            }
            this->position->Next();
        }

        /*
         * Determines whether the current character conforms to the character specified by the grammar
         * 判断当前字符是否符合文法规定的字符
         */
        switch (this->position->CurrentChar) {
            case '\0':
                this->MakeToken(TokenType::EOF_);
                break;
            case '+':
                this->MakeToken(TokenType::PLUS);
                this->position->Next();
                break;
            case '-':
                this->MakeToken(TokenType::MINUS);
                this->position->Next();
                break;
            case '*':
                this->MakeToken(TokenType::STAR);
                this->position->Next();
                break;
            case '/':
                this->MakeToken(TokenType::SLASH);
                this->position->Next();
                break;
            case '(':
                this->MakeToken(TokenType::SLPAREN);
                this->position->Next();
                break;
            case ')':
                this->MakeToken(TokenType::SRPAREN);
                this->position->Next();
                break;
            case '{':
                this->MakeToken(TokenType::BLPAREN);
                this->position->Next();
                break;
            case '}':
                this->MakeToken(TokenType::BRPAREN);
                this->position->Next();
                break;
            case ',':
                this->MakeToken(TokenType::COMMA);
                this->position->Next();
                break;
            case '=':
                switch (this->PeekChar(1)) {
                    case '=':
                        this->position->Next();
                        this->MakeToken(TokenType::EQUAL);
                        break;
                    case '<':
                        this->position->Next();
                        this->MakeToken(TokenType::LESSER_OR_EQUAL);
                        break;
                    default:
                        this->MakeToken(TokenType::ASSIGN);
                        break;
                }
                this->position->Next();
                break;
            case '!':
                if (this->PeekChar(1) == '=') {
                    this->position->Next();
                    this->MakeToken(TokenType::NOT_EQUAL);
                }
                this->position->Next();
                break;
            case '>':
                if (this->PeekChar(1) == '=') {
                    this->position->Next();
                    this->MakeToken(TokenType::GREATER_OR_EQUAL);
                } else {
                    this->MakeToken(TokenType::GREATER);
                }
                this->position->Next();
                break;
            case '<':
                if (this->PeekChar(1) == '=') {
                    this->position->Next();
                    this->MakeToken(TokenType::LESSER_OR_EQUAL);
                } else {
                    this->MakeToken(TokenType::LESSER);
                }
                this->position->Next();
                break;
            case ';':
                this->MakeToken(TokenType::SEMICOLON);
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
                DiagException(this->position, "current char \'%c\' is illegal", this->position->CurrentChar);
                break;
        }
    }

    void Lexer::MakeToken(TokenType type, std::string_view value) {
        this->CurrentToken = std::make_shared<Token>(type, value);
    }

    void Lexer::MakeToken(TokenType type) {
        this->MakeToken(type, "");
    }

    void Lexer::NumberToken() {
        int start = this->position->Index;

        while (this->IsDigit()) {
            this->position->Next();
        }

        this->MakeToken(TokenType::VT_NUMBER, this->position->Source.substr(start, this->position->Index - start));
    }

    void Lexer::IdentifierToken() {
        int start = this->position->Index;

        while (this->IsLetterOrDigit()) {
            this->position->Next();
        }

        std::string_view value = this->position->Source.substr(start, this->position->Index - start);

        if (value == "if") {
            this->MakeToken(TokenType::IF);
            return;
        }

        if (value == "else") {
            this->MakeToken(TokenType::ELSE);
            return;
        }

        if (value == "while") {
            this->MakeToken(TokenType::WHILE);
            return;
        }

        if (value == "for") {
            this->MakeToken(TokenType::FOR);
            return;
        }

        if (value == "return") {
            this->MakeToken(TokenType::RETURN);
            return;
        }

        if (value == "int") {
            this->MakeToken(TokenType::VT_NUMBER);
            return;
        }

        this->MakeToken(TokenType::IDENTIFIER, value);
    }

    bool Lexer::IsLetter() {
        return (this->position->CurrentChar >= 'a' && this->position->CurrentChar <= 'z') || (this->position->CurrentChar >= 'A' && this->position->CurrentChar <= 'Z') || this->position->CurrentChar == '_';
    }

    bool Lexer::IsDigit() {
        return std::isdigit(this->position->CurrentChar);
    }

    bool Lexer::IsLetterOrDigit() {
        return this->IsLetter() || this->IsDigit();
    }

    void Lexer::ExpectToken(TokenType type) {
        if (this->CurrentToken->type == type) {
            this->Next();
        } else {
            DiagException(this->position, "\'%s\' expected", Front::Lexer::GetSpelling(type));
        }
    }

    const char *Lexer::GetSpelling(TokenType type) {
        switch (type) {
            case TokenType::PLUS:
                return "+";
            case TokenType::MINUS:
                return "-";
            case TokenType::STAR:
                return "*";
            case TokenType::SLASH:
                return "-";
            case TokenType::SLPAREN:
                return "(";
            case TokenType::SRPAREN:
                return ")";
            case TokenType::ASSIGN:
                return "=";
            case TokenType::SEMICOLON:
                return ";";
            case TokenType::EQUAL:
                return "==";
            case TokenType::NOT_EQUAL:
                return "!=";
            case TokenType::GREATER:
                return ">";
            case TokenType::GREATER_OR_EQUAL:
                return ">=";
            case TokenType::LESSER:
                return "<";
            case TokenType::LESSER_OR_EQUAL:
                return "<=";
            case TokenType::EOF_:
                return "EOF";
            default:
                return nullptr;
        }
    }

    char Lexer::PeekChar(int count) {
        assert(count >= 0);
        int i = this->position->Index + count;
        return (i < this->position->Source.length()) ? this->position->Source[i] : '\0';
    }

    void Lexer::BeginPeekToken() {
        this->peekPosition->CurrentChar = this->position->CurrentChar;
        this->peekPosition->Index = this->position->Index;
        this->peekPosition->x = this->position->x;
        this->peekPosition->y = this->position->y;
        this->peekPosition->LineX = this->position->LineX;
        this->CurrentPeekToken = this->CurrentToken;
    }

    void Lexer::EndPeekToken() {
        this->position->CurrentChar = this->peekPosition->CurrentChar;
        this->position->Index = this->peekPosition->Index;
        this->position->x = this->peekPosition->x;
        this->position->y = this->peekPosition->y;
        this->position->LineX = this->peekPosition->LineX;
        this->CurrentToken = this->CurrentPeekToken;
    }

} //` Front