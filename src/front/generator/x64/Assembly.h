/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_ASSEMBLY_H
#define KNIFE_ASSEMBLY_H

#include "front/AST.h"

namespace Front {

    class Assembly : public NodeVisitor {
    private:
        int stackLevel {0};
    public:
        void goProgram(ProgramNode *node) override;
    private:
        void goBinary(BinaryNode *node) override;
        void goConstant(ConstantNode *node) override;

        void Push();
        void Pop(const char* reg);
    };

} // Front

#endif //KNIFE_ASSEMBLY_H
