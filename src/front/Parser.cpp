/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Parser.h"
#include <cassert>

namespace Front {
    Parser::Parser(Lexer &lexer) : lexer(lexer) {
        // ignore
    }

    std::shared_ptr<ProgramNode> Parser::Parse() {
        auto node = std::make_shared<ProgramNode>();

        this->variables = &node->variables;

        while (this->lexer.currentToken->type != TokenType::EOF_) {
            node->statements.push_back(this->Statement());
        }

        return node;
    }

    std::shared_ptr<ASTNode> Parser::Statement() {
        auto node = std::make_shared<StatementNode>();
        node->left = this->Expression();
        assert(this->lexer.currentToken->type == TokenType::SEMICOLON);
        this->lexer.Next();
        return node;
    }

    std::shared_ptr<ASTNode> Parser::Assgin() {
        auto left = this->Term();

        if (this->lexer.currentToken->type == TokenType::EQUAL) {
            this->lexer.Next();
            auto node = std::make_shared<AssignNode>();
            node->left = left;
            node->right = this->Assgin();
            return node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Expression() {
        return this->Assgin();
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
            this->lexer.ExpectToken(TokenType::SRPAREN);
            return node;
        }

        if (this->lexer.currentToken->type == TokenType::VT_NUMBER) {
            auto node = std::make_shared<ConstantNode>();
            node->value = std::stoi(this->lexer.currentToken->value.data());
            this->lexer.Next();
            return node;
        }

        if (this->lexer.currentToken->type == TokenType::IDENTIFIER) {
            auto node = std::make_shared<VariableNode>();
            std::shared_ptr<Variable> object = this->FindVariable(this->lexer.currentToken->value);

            if (!object) {
                object = this->MakeVariable(this->lexer.currentToken->value);
            }

            node->variableObject = object;
            this->lexer.Next();
            return node;
        }

        //throw
        return nullptr;

    }

    std::shared_ptr<Variable> Parser::FindVariable(std::string_view name) {
        return (this->variableMap.find(name) != this->variableMap.end()) ? variableMap[name] : nullptr;
    }

    std::shared_ptr<Variable> Parser::MakeVariable(std::string_view name) {
        auto variable = std::make_shared<Variable>();
        variable->name = name;
        variable->offset = 0;
        this->variables->push_front(variable);
        this->variableMap[name] = variable;
        return variable;
    }
} // Front