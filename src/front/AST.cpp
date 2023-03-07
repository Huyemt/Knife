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
}