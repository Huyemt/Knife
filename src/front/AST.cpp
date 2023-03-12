/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "AST.h"

namespace Front {
    void ProgramNode::Accept(NodeVisitor *visitor) {
        visitor->goProgram(this);
    }

    void StatementNode::Accept(NodeVisitor *visitor) {
        visitor->goStatement( this);
    }

    void BinaryNode::Accept(NodeVisitor *visitor) {
        visitor->goBinary(this);
    }

    void ConstantNode::Accept(NodeVisitor *visitor) {
        visitor->goConstant(this);
    }

    void VariableNode::Accept(NodeVisitor *visitor) {
        visitor->goVariable(this);
    }

    void AssignNode::Accept(NodeVisitor *visitor) {
        visitor->goAssign(this);
    }

    void IfNode::Accept(NodeVisitor *visitor) {
        visitor->goIf(this);
    }

    void BlockNode::Accept(NodeVisitor *visitor) {
        visitor->goBlock(this);
    }

    void WhileNode::Accept(NodeVisitor *visitor) {
        visitor->goWhile(this);
    }

    void ForNode::Accept(NodeVisitor *visitor) {
        visitor->goFor(this);
    }

    void FunctionNode::Accept(NodeVisitor *visitor) {
        visitor->goFunction(this);
    }

    void FunctionCallNode::Accept(NodeVisitor *visitor) {
        visitor->goFunctionCall(this);
    }

    void FunctionReturnNode::Accept(NodeVisitor *visitor) {
        visitor->goFunctionReturn(this);
    }

    void DeclarationNode::Accept(NodeVisitor *visitor) {
        visitor->goDeclaration(this);
    }

    void StatementExperssionNode::Accept(NodeVisitor *visitor) {
        visitor->goStatementExperssion(this);
    }

    void UnaryNode::Accept(NodeVisitor *visitor) {
        visitor->goUnary(this);
    }
}