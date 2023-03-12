/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Parser.h"

#include <utility>
#include "Lexer.h"
#include "Diag.h"
#include "Type.h"

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

        auto ty = this->DeclarationSpec();
        std::shared_ptr<Token> token1;
        ty = this->Declarator(ty, token1);

        node->Name = token1->value;
        node->Base = ty;

        std::shared_ptr<FunctionReturn> func = std::dynamic_pointer_cast<FunctionReturn>(ty);
        for (auto it = func->Params.rbegin(); it != func->Params.rend(); ++it) {
            node->Params.push_front(this->MakeVariable((*it)->Token->value, (*it)->Base));
        }

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

        if (this->IsTypeName()) {
            auto node = std::make_shared<DeclarationNode>();
            auto ty = this->DeclarationSpec();
            int i = 0;

            while (this->lexer.CurrentToken->type != TokenType::SEMICOLON) {
                if (i > 0) {
                    this->lexer.ExpectToken(TokenType::SEMICOLON);
                }

                std::shared_ptr<Token> token;
                ty = this->Declarator(ty, token);

                if (this->lexer.CurrentToken->type != TokenType::ASSIGN) {
                    continue;
                }

                auto variable = this->MakeVariable(token->value, ty);
                i++;

                this->lexer.Next();

                auto assign = std::make_shared<AssignNode>();
                assign->Left = this->MakeNodeVariable(variable);
                assign->Right = this->Expression();
                node->Assigns.push_back(assign);
            }

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
        std::shared_ptr<ASTNode> left = this->Unary();

        // *
        // /
        while (this->lexer.CurrentToken->type == TokenType::STAR || this->lexer.CurrentToken->type == TokenType::SLASH) {
            BinaryOperator anOperator = (this->lexer.CurrentToken->type == TokenType::STAR) ? BinaryOperator::MUL : BinaryOperator::DIV;
            this->lexer.Next();
            auto node = std::make_shared<BinaryNode>();
            node->anOperator = anOperator;
            node->Left = left;
            node->Right = this->Unary();
            left = node;
        }

        return left;
    }

    std::shared_ptr<ASTNode> Parser::Primary() {
        if (this->lexer.CurrentToken->type == TokenType::SLPAREN) {
            this->lexer.BeginPeekToken();
            this->lexer.Next();

            if (this->lexer.CurrentToken->type == TokenType::BLPAREN) {
                this->lexer.EndPeekToken();
                this->lexer.ExpectToken(TokenType::SLPAREN);
                this->lexer.ExpectToken(TokenType::BLPAREN);

                auto node = std::make_shared<StatementExperssionNode>();
                while (this->lexer.CurrentToken->type != TokenType::BRPAREN) {
                     node->Statements.push_back(this->Statement());
                }

                this->lexer.Next();
                this->lexer.ExpectToken(TokenType::SRPAREN);

                return node;
            }

            this->lexer.EndPeekToken();

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
                DiagException(this->lexer.position, "undefined variable");
            }

            node->VariableObject = object;
            this->lexer.Next();
            return node;
        }

        DiagException(this->lexer.position, "not supported token");

        return nullptr;

    }

    std::shared_ptr<Type> Parser::DeclarationSpec() {
        if (this->lexer.CurrentToken->type == TokenType::VT_NUMBER) {
            this->lexer.Next();
            return Type::INT;
        }
        DiagException(this->lexer.position, "this type is no support");
        return nullptr;
    }

    std::shared_ptr<Type> Parser::Declarator(const std::shared_ptr<Type>& base, std::shared_ptr<Token> &token) {
        const auto& ty = base;

        if (this->lexer.CurrentToken->type != TokenType::IDENTIFIER) {
            DiagException(this->lexer.position, "expected a variable name");
        }
        token = this->lexer.CurrentToken;

        this->lexer.ExpectToken(TokenType::IDENTIFIER);

        return this->TypeSuffix(ty);
    }

    std::shared_ptr<Type> Parser::TypeSuffix(const std::shared_ptr<Type>& base) {
        if (this->lexer.CurrentToken->type == TokenType::SLPAREN) {
            this->lexer.Next();
            auto func = std::make_shared<FunctionReturn>(base);

            if (this->lexer.CurrentToken->type != TokenType::SRPAREN) {
                std::shared_ptr<Token> token;
                auto ty = this->Declarator(this->DeclarationSpec(), token);
                auto param = std::make_shared<Param>();
                param->Base = ty;
                param->Token = token;
                func->Params.push_back(param);

                while (this->lexer.CurrentToken->type != TokenType::SRPAREN) {
                    this->lexer.ExpectToken(TokenType::COMMA);

                    ty = this->Declarator(this->DeclarationSpec(), token);
                    param = std::make_shared<Param>();
                    param->Base = ty;
                    param->Token = token;
                    func->Params.push_back(param);
                }
            }

            this->lexer.ExpectToken(TokenType::SRPAREN);
            return func;
        }

        return base;
    }

    std::shared_ptr<Variable> Parser::FindVariable(std::string_view name) {
        return (this->variableMap.find(name) != this->variableMap.end()) ? variableMap[name] : nullptr;
    }

    std::shared_ptr<Variable> Parser::MakeVariable(std::string_view name, std::shared_ptr<Type> ty) {
        auto variable = std::make_shared<Variable>();
        variable->Base = std::move(ty);
        variable->Name = name;
        variable->Offset = 0;
        this->variables->push_front(variable);
        this->variableMap[name] = variable;
        return variable;
    }

    bool Parser::IsTypeName() {
        switch (this->lexer.CurrentToken->type) {
            case TokenType::VT_NUMBER:
                return true;
            default:
                return false;
        }
    }

    std::shared_ptr<ASTNode> Parser::MakeNodeVariable(const std::shared_ptr<Variable>& variable) {
        auto node = std::make_shared<VariableNode>();
        node->VariableObject = variable;
        node->Base = variable->Base;
        return node;
    }

    std::shared_ptr<ASTNode> Parser::Unary() {
        switch (this->lexer.CurrentToken->type) {
            case TokenType::PLUS: {
                auto node = std::make_shared<UnaryNode>();
                node->anOperator = UnaryOperator::PLUS;
                this->lexer.Next();
                node->Left = this->Unary();
                return node;
            }
            case TokenType::MINUS:{
                auto node = std::make_shared<UnaryNode>();
                node->anOperator = UnaryOperator::MINUS;
                this->lexer.Next();
                node->Left = this->Unary();
                return node;
            }
            default:
                return this->Primary();
        }
    }
} // Front