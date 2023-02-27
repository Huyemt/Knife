/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "Assembly.h"
#include <cstdio>
#include <cassert>

namespace Front {
    void Assembly::goProgram(ProgramNode *node) {
        printf("\t.text\n");
#ifdef __linux__
        printf("\t.globl prog\n");
		printf("prog:\n")
#else
        // macos
        printf("\t.globl _prog\n");
        printf("_prog:\n");
#endif // __linux__

        printf("\tpush %%rbp\n");
        printf("\tmov %%rsp, %%rbp\n");
        printf("\tsub $32, %%rsp\n");

        node->left->Accept(this);
        assert(this->stackLevel == 0);

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
} // Front