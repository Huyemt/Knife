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

        while (this->lexer.CurrentToken->type != TokenType::EOF_) {
            node->Functions.push_back(this->Function());
        }

        return node;
    }

    std::shared_ptr<ASTNode> Parser::Function() {
        auto node = std::make_shared<FunctionNode>();
        this->variables = &node->Variables;
        this->variableMap.clear();

        node->Name = this->lexer.CurrentToken->value;

        this->lexer.ExpectToken(TokenType::IDENTIFIER);
        this->lexer.ExpectToken(TokenType::SLPAREN);

        if (this->lexer.CurrentToken->type != TokenType::SRPAREN) {
            auto token = this->lexer.CurrentToken;

            this->Primary();

            node->Params.push_back(this->variableMap[token->value]);

            while (this->lexer.CurrentToken->type == TokenType::COMMA) {
                this->lexer.Next();

                token = this->lexer.CurrentToken;

                this->Primary();

                node->Params.push_back(this->variableMap[token->value]);
            }
        }

        this->lexer.ExpectToken(TokenType::SRPAREN);
        this->lexer.ExpectToken(TokenType::BLPAREN);

        while (this->lexer.CurrentToken->type != TokenType::BRPAREN) {
            node->Statements.push_back(this->Statement());
        }

        this->lexer.ExpectToken(TokenType::BRPAREN);

        return node;
    }

    std::shared_ptr<ASTNode> Parser::FunctionCall() {
        auto node = std::make_shared<FunctionCallNode>();
        node->Name = this->lexer.CurrentToken->value;
        this->lexer.ExpectToken(TokenType::IDENTIFIER);
        this->lexer.ExpectToken(TokenType::SLPAREN);

        if (this->lexer.CurrentToken->type != TokenType::SRPAREN) {
            node->Params.push_back(this->Assgin());
            while (this->lexer.CurrentToken->type == TokenType::COMMA) {
                this->lexer.Next();
                node->Params.push_back(this->Assgin());
            }
        }

        this->lexer.ExpectToken(TokenType::SRPAREN);
        return node;
    }

    std::shared_ptr<ASTNode> Parser::Statement() {
        if (this->lexer.CurrentToken->type == TokenType::IF) {
            this->lexer.Next();
            this->lexer.ExpectToken(TokenType::SLPAREN);
            auto node = std::make_shared<IfNode>();
            node->Condition = this->Expression();
            this->lexer.ExpectToken(TokenType::SRPAREN);
            node->Then = this->Statement();
            if (this->lexer.CurrentToken->type == TokenType::ELSE) {
                this->lexer.Next();
                node->Else = this->Statement();
            }
            return node;
        }

        if (this->lexer.CurrentToken->type == TokenType::WHILE) {
            this->lexer.Next();
            this->lexer.ExpectToken(TokenType::SLPAREN);
            auto node = std::make_shared<WhileNode>();
            node->Condition = this->Expression();
            this->lexer.ExpectToken(TokenType::SRPAREN);
            node->Then = this->Statement();
            if (this->lexer.CurrentToken->type == TokenType::ELSE) {
                this->lexer.Next();
                node->Else = this->Statement();
            }
            return node;
        }

        if (this->lexer.CurrentToken->type == TokenType::FOR) {
            this->lexer.Next();
            this->lexer.ExpectToken(TokenType::SLPAREN);

            auto node = std::make_shared<ForNode>();

            if (this->lexer.CurrentToken->type != TokenType::SEMICOLON) {
                node->Init = this->Expression();
            }
            this->lexer.ExpectToken(TokenType::SEMICOLON);

            if (this->lexer.CurrentToken->type != TokenType::SEMICOLON) {
                node->Condition = this->Expression();
            }
            this->lexer.ExpectToken(TokenType::SEMICOLON);

            if (this->lexer.CurrentToken->type != TokenType::SRPAREN) {
                node->Operate = this->Expression();
            }
            this->lexer.ExpectToken(TokenType::SRPAREN);

            node->Then = this->Statement();

            if (this->lexer.CurrentToken->type == TokenType::ELSE) {
                this->lexer.Next();
                node->Else = this->Statement();
            }

            return node;
        }

        if (this->lexer.CurrentToken->type == TokenType::BLPAREN) {
            this->lexer.Next();
            auto node = std::make_shared<BlockNode>();
            while (this->lexer.CurrentToken->type != TokenType::BRPAREN) {
                node->Statements.push_back(this->Statement());
            }
            this->lexer.ExpectToken(TokenType::BRPAREN);
            return node;
        }

        if (this->lexer.CurrentToken->type == TokenType::RETURN) {
            this->lexer.Next();
            auto node = std::make_shared<FunctionReturnNode>();
            node->Left = this->Expression();
            this->lexer.ExpectToken(TokenType::SEMICOLON);
            return node;
        }

        auto node = std::make_shared<StatementNode>();
        if (this->lexer.CurrentToken->type != TokenType::SEMICOLON) {
            node->Left = this->Expression();
        }
        this->lexer.ExpectToken(TokenType::SEMICOLON);
        return node;
    }

    std::shared_ptr<ASTNode> Parser::Assgin() {
        auto left = this->Equal();

        if (this->lexer.CurrentToken->type == TokenType::ASSIGN) {
            this->lexer.Next();
            auto node = std::make_shared<AssignNode>();
            node->Left = left;
            node->Right = this->Assgin();
            return node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Equal() {
        auto left = this->Relational();
        while (this->lexer.CurrentToken->type == TokenType::EQUAL || this->lexer.CurrentToken->type == TokenType::NOT_EQUAL) {
            BinaryOperator anOperator = (this->lexer.CurrentToken->type == TokenType::EQUAL) ? BinaryOperator::EQUAL : BinaryOperator::NOT_EQUAL;
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->Left = left;
            node->Right = this->Relational();
            left = node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Relational() {
        auto left = this->Term();
        while (this->lexer.CurrentToken->type == TokenType::GREATER || this->lexer.CurrentToken->type == TokenType::GREATER_OR_EQUAL || this->lexer.CurrentToken->type == TokenType::LESSER || this->lexer.CurrentToken->type == TokenType::LESSER_OR_EQUAL) {
            BinaryOperator anOperator = BinaryOperator::GREATER;
            if (this->lexer.CurrentToken->type == TokenType::GREATER_OR_EQUAL) {
                anOperator = BinaryOperator::GREATER_OR_EQUAL;
            } else if (this->lexer.CurrentToken->type == TokenType::LESSER) {
                anOperator = BinaryOperator::LESSER;
            } else if (this->lexer.CurrentToken->type == TokenType::LESSER_OR_EQUAL){
                anOperator = BinaryOperator::LESSER_OR_EQUAL;
            }
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->Left = left;
            node->Right = this->Term();
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
        while (this->lexer.CurrentToken->type == TokenType::PLUS || this->lexer.CurrentToken->type == TokenType::MINUS) {
            BinaryOperator anOperator = (this->lexer.CurrentToken->type == TokenType::PLUS) ? BinaryOperator::ADD : BinaryOperator::SUB;
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->Left = left;
            node->Right = this->Factor();
            left = node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Factor() {
        std::shared_ptr<ASTNode> left = this->Primary();

        // *
        // /
        while (this->lexer.CurrentToken->type == TokenType::MUL || this->lexer.CurrentToken->type == TokenType::DIV) {
            BinaryOperator anOperator = (this->lexer.CurrentToken->type == TokenType::MUL) ? BinaryOperator::MUL : BinaryOperator::DIV;
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->Left = left;
            node->Right = this->Primary();
            left = node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Primary() {
        if (this->lexer.CurrentToken->type == TokenType::SLPAREN) {
            this->lexer.Next();
            auto node = this->Expression();
            this->lexer.ExpectToken(TokenType::SRPAREN);
            return node;
        }

        if (this->lexer.CurrentToken->type == TokenType::VT_NUMBER) {
            auto node = std::make_shared<ConstantNode>();
            node->Value = std::stoi(this->lexer.CurrentToken->value.data());
            this->lexer.Next();
            return node;
        }

        if (this->lexer.CurrentToken->type == TokenType::IDENTIFIER) {
            this->lexer.BeginPeekToken();
            this->lexer.Next();

            if (this->lexer.CurrentToken->type == TokenType::SLPAREN) {
                this->lexer.EndPeekToken();
                return this->FunctionCall();
            }

            this->lexer.EndPeekToken();

            auto node = std::make_shared<VariableNode>();
            std::shared_ptr<Variable> object = this->FindVariable(this->lexer.CurrentToken->value);

            if (!object) {
                object = this->MakeVariable(this->lexer.CurrentToken->value);
            }

            node->VariableObject = object;
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
        variable->Name = name;
        variable->Offset = 0;
        this->variables->push_front(variable);
        this->variableMap[name] = variable;
        return variable;
    }
} // Front