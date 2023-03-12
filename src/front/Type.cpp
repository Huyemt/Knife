/*
 * Author: Huyemt
 * Time: 2023/3/11
 */

#include "Type.h"
#include "AST.h"
#include <cassert>

namespace Front {
    std::shared_ptr<BuiltIn> Type::INT = std::make_shared<BuiltIn>(BuiltIn::DType::INT ,8, 8);

    BuiltIn::DType BuiltIn::GetDataType() const {
        return this->dType;
    }

    bool Type::IsInteger() const {
        if (this->DataType == TypeClass::BUILTIN) {
            auto t = dynamic_cast<const BuiltIn*>(this);
            return t->GetDataType() == BuiltIn::DType::INT;
        }

        return false;
    }

    bool Type::IsFunction() const {
        return this->DataType == TypeClass::FUNCTION;
    }

    void TypeVisitor::goProgram(ProgramNode *node) {
        for (auto &f : node->Functions) {
            f->Accept(this);
        }
    }

    void TypeVisitor::goFunction(FunctionNode *node) {
        for (auto &s : node->Statements) {
            s->Accept(this);
        }
    }

    void TypeVisitor::goFunctionCall(FunctionCallNode *node) {
        for (auto &p : node->Params) {
            p->Accept(this);
        }

        node->Base = Type::INT;
    }

    void TypeVisitor::goStatement(StatementNode *node) {
        if (node->Left) {
            node->Left->Accept(this);
            node->Base = node->Left->Base;
        }
    }

    void TypeVisitor::goIf(IfNode *node) {
        node->Condition->Accept(this);
        node->Then->Accept(this);
        if (node->Else) {
            node->Else->Accept(this);
        }
    }

    void TypeVisitor::goWhile(WhileNode *node) {
        node->Condition->Accept(this);
        node->Then->Accept(this);
        if (node->Else) {
            node->Else->Accept(this);
        }
    }

    void TypeVisitor::goFor(ForNode *node) {
        if (node->Init) {
            node->Init->Accept(this);
        }

        if (node->Condition) {
            node->Condition->Accept(this);
        }

        node->Then->Accept(this);

        if (node->Operate) {
            node->Operate->Accept(this);
        }

        if (node->Else) {
            node->Else->Accept(this);
        }
    }

    void TypeVisitor::goBlock(BlockNode *node) {
        for (auto &s : node->Statements) {
            s->Accept(this);
        }
    }

    void TypeVisitor::goFunctionReturn(FunctionReturnNode *node) {
        node->Left->Accept(this);
        node->Base = node->Left->Base;
    }

    void TypeVisitor::goStatementExperssion(StatementExperssionNode *node) {
        for (auto &s : node->Statements) {
            s->Accept(this);
        }
    }

    void TypeVisitor::goDeclaration(DeclarationNode *node) {
        for (auto &n : node->Assigns) {
            n->Accept(this);
        }
    }

    void TypeVisitor::goAssign(AssignNode *node) {
        node->Left->Accept(this);
        node->Right->Accept(this);
        node->Base = node->Left->Base;
    }

    void TypeVisitor::goBinary(BinaryNode *node) {
        node->Left->Accept(this);
        node->Right->Accept(this);

        switch (node->anOperator)
        {
            case BinaryOperator::ADD:
            case BinaryOperator::SUB:
            case BinaryOperator::MUL:
            case BinaryOperator::DIV:
                node->Base = node->Left->Base;
                break;
            case BinaryOperator::EQUAL:
            case BinaryOperator::NOT_EQUAL:
            case BinaryOperator::GREATER:
            case BinaryOperator::GREATER_OR_EQUAL:
            case BinaryOperator::LESSER:
            case BinaryOperator::LESSER_OR_EQUAL:
                node->Base = Type::INT;
                break;
            default:
                assert(0);
                break;
        }
    }

    void TypeVisitor::goConstant(ConstantNode *node) {
        node->Base = Type::INT;
    }

    void TypeVisitor::goVariable(VariableNode *node) {
        node->Base = node->VariableObject->Base;
    }

    void TypeVisitor::goUnary(UnaryNode *node) {
        switch (node->anOperator) {
            case UnaryOperator::PLUS:
            case UnaryOperator::MINUS: {
                node->Base = node->Left->Base;
                break;
            }
        }
    }
} // Front