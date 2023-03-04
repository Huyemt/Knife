/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Parser.h"

namespace Front {
    Parser::Parser(Lexer &lexer) : lexer(lexer) {
        // ignore
    }

    std::shared_ptr<ProgramNode> Parser::Parse() {
        auto node = std::make_shared<ProgramNode>();
        node->left = this->Expression();
        return node;
    }

    std::shared_ptr<ASTNode> Parser::Expression() {
        return this->Term();
    }

    std::shared_ptr<ASTNode> Parser::Term() {
        std::shared_ptr<ASTNode> left = this->Factor();

        // +
        // -
        while (this->lexer.currentToken->type == TokenType::PLUS || this->lexer.currentToken->type == TokenType::MINUS) {
            BinaryOperator anOperator = (this->lexer.currentToken->type == TokenType::PLUS) ? BinaryOperator::ADD : BinaryOperator::SUB;
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->left = left;
            node->right = this->Factor();
            left = node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Factor() {
        std::shared_ptr<ASTNode> left = this->Primary();

        // *
        // /
        while (this->lexer.currentToken->type == TokenType::MUL || this->lexer.currentToken->type == TokenType::DIV) {
            BinaryOperator anOperator = (this->lexer.currentToken->type == TokenType::MUL) ? BinaryOperator::MUL : BinaryOperator::DIV;
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->left = left;
            node->right = this->Primary();
            left = node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Primary() {
        if (this->lexer.currentToken->type == TokenType::SLPAREN) {
            this->lexer.Next();
            auto node = this->Expression();
            this->lexer.Next();
            return node;
        }

        auto node = std::make_shared<ConstantNode>();
        if (this->lexer.currentToken->type == TokenType::VT_NUMBER) {
            node->value = std::stoi(this->lexer.currentToken->value.data());
        }
        this->lexer.Next();
        return node;
    }
} // Front