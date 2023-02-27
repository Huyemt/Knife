/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "AST.h"

namespace Front {
    void ProgramNode::Accept(NodeVisitor *visitor) {
        visitor->goProgram(this);
    }

    void BinaryNode::Accept(NodeVisitor *visitor) {
        visitor->goBinary(this);
    }

    void ConstantNode::Accept(NodeVisitor *visitor) {
        visitor->goConstant(this);
    }
}