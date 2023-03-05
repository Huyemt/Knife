/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Parser.h"
#include "Lexer.h"
#include "Diag.h"

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
        if (this->lexer.currentToken->type == TokenType::IF) {
            auto node = std::make_shared<IfNode>();
            this->lexer.Next();
            this->lexer.ExpectToken(TokenType::SLPAREN);
            node->Condition = this->Expression();
            this->lexer.ExpectToken(TokenType::SRPAREN);
            node->Then = this->Statement();
            if (this->lexer.currentToken->type == TokenType::ELSE) {
                this->lexer.Next();
                node->Else = this->Statement();
            }
            return node;
        } else {
            auto node = std::make_shared<StatementNode>();
            node->left = this->Expression();
            this->lexer.ExpectToken(TokenType::SEMICOLON);
            return node;
        }
    }

    std::shared_ptr<ASTNode> Parser::Assgin() {
        auto left = this->Equal();

        if (this->lexer.currentToken->type == TokenType::ASSIGN) {
            this->lexer.Next();
            auto node = std::make_shared<AssignNode>();
            node->left = left;
            node->right = this->Assgin();
            return node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Equal() {
        auto left = this->Relational();
        while (this->lexer.currentToken->type == TokenType::EQUAL || this->lexer.currentToken->type == TokenType::NOT_EQUAL) {
            BinaryOperator anOperator = (this->lexer.currentToken->type == TokenType::EQUAL) ? BinaryOperator::EQUAL : BinaryOperator::NOT_EQUAL;
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->left = left;
            node->right = this->Relational();
            left = node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Relational() {
        auto left = this->Term();
        while (this->lexer.currentToken->type == TokenType::GREATER || this->lexer.currentToken->type == TokenType::GREATER_OR_EQUAL || this->lexer.currentToken->type == TokenType::LESSER || this->lexer.currentToken->type == TokenType::LESSER_OR_EQUAL) {
            BinaryOperator anOperator = BinaryOperator::GREATER;
            if (this->lexer.currentToken->type == TokenType::GREATER_OR_EQUAL) {
                anOperator = BinaryOperator::GREATER_OR_EQUAL;
            } else if (this->lexer.currentToken->type == TokenType::LESSER) {
                anOperator = BinaryOperator::LESSER;
            } else if (this->lexer.currentToken->type == TokenType::LESSER_OR_EQUAL){
                anOperator = BinaryOperator::LESSER_OR_EQUAL;
            }
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->left = left;
            node->right = this->Term();
            left = node;
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

        DiagException(this->lexer.position, "not supported token");

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