/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_ASSEMBLY_H
#define KNIFE_ASSEMBLY_H

#include "front/AST.h"

namespace Front {
    /**
     * AT&T style
     * x64 Assembly
     */
    class Assembly : public NodeVisitor {
    public:
        void goProgram(ProgramNode *node) override;
    private:
        int stackLevel {0};
        void goStatement(StatementNode *node) override;
        void goBinary(BinaryNode *node) override;
        void goConstant(ConstantNode *node) override;
        void goVariable(VariableNode *node) override;
        void goAssign(AssignNode *node) override;

        void Push();
        void Pop(const char* reg);
    };

} // Front

#endif //KNIFE_ASSEMBLY_H
