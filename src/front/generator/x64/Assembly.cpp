/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Assembly.h"
#include <cstdio>
#include <cassert>

namespace Front {
    void Assembly::goProgram(ProgramNode *node) {
        printf(".text\n");
        printf(".globl prog\n");
        printf("prog:\n");

        /*
         * Space allocation
         * 空间分配
         */
        int stackSize = 0;
        for (auto &v : node->variables) {
            stackSize += 8;
            v->offset = -stackSize;
        }

        printf("\tpush %%rbp\n");
        printf("\tmov %%rsp, %%rbp\n");
        printf("\tsub $%d, %%rsp\n", stackSize);

        for (auto &s : node->statements) {
            s->Accept(this);
            assert(this->stackLevel == 0);
        }

        printf("\tmov %%rbp, %%rsp\n");
        printf("\tpop %%rbp\n");
        printf("\tret\n");
    }

    void Assembly::goBinary(BinaryNode *node) {
        node->right->Accept(this);
        this->Push();
        node->left->Accept(this);
        this->Pop("%rdi");

        switch (node->anOperator)
        {
            case BinaryOperator::ADD:
                printf("\tadd %%rdi, %%rax\n");
                break;
            case BinaryOperator::SUB:
                printf("\tsub %%rdi, %%rax\n");
                break;
            case BinaryOperator::MUL:
                printf("\timul %%rdi, %%rax\n");
                break;
            case BinaryOperator::DIV:
                printf("\tcqo\n");
                printf("\tidiv %%rdi\n");
                break;
            default:
                assert(0);
                break;
        }
    }

    void Assembly::goConstant(ConstantNode *node) {
        printf("\tmov $%d, %%rax\n", node->value);
    }

    void Assembly::Push() {
        printf("\tpush %%rax\n");
        this->stackLevel++;
    }

    void Assembly::Pop(const char *reg) {
        printf("\tpop %s\n", reg);
        this->stackLevel--;
    }

    void Assembly::goStatement(StatementNode *node) {
        node->left->Accept(this);
    }

    void Assembly::goAssign(AssignNode *node) {
        auto vNode = std::dynamic_pointer_cast<VariableNode>(node->left);
        assert(vNode != nullptr);
        printf("\tlea %d(%%rbp), %%rax\n", vNode->variableObject->offset);
        this->Push();
        node->right->Accept(this);
        this->Pop("%rdi");
        printf("\tmov %%rax, (%%rdi)\n");
    }

    void Assembly::goVariable(VariableNode *node) {
        printf("\tlea %d(%%rbp), %%rax\n", node->variableObject->offset);
        printf("\tmov (%%rax), %%rax\n");
    }
} // Front