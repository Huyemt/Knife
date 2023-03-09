/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_X64_H
#define KNIFE_X64_H

#include "front/AST.h"

namespace Front {
    /**
     * AT&T style
     * assembly x64
     */
    class x64 : public NodeVisitor {
    public:
        void goProgram(ProgramNode *node) override;
    private:
        int stackLevel {0};
        int IfSequence {0};
        int LoopSequence {0};
        const char* RegX64[6] = {
                "%rdi",
                "%rsi",
                "%rdx",
                "%rcx",
                "%r8",
                "%r9"
        };
        std::string CurrentFunctionName;

        void goStatement(StatementNode *node) override;
        void goBinary(BinaryNode *node) override;
        void goConstant(ConstantNode *node) override;
        void goVariable(VariableNode *node) override;
        void goAssign(AssignNode *node) override;
        void goBlock(BlockNode *node) override;
        void goIf(IfNode *node) override;
        void goWhile(WhileNode *node) override;
        void goFor(ForNode *node) override;
        void goFunction(FunctionNode *node) override;
        void goFunctionReturn(FunctionReturnNode *node) override;
        void goFunctionCall(FunctionCallNode *node) override;

        void Push();
        void Pop(const char* reg);
        int AlignTo(int size, int align);
    };

} // Front

#endif //KNIFE_X64_H
