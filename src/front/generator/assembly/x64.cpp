/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "x64.h"
#include <cstdio>
#include <cassert>

namespace Front {
    void x64::goProgram(ProgramNode *node) {
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

    void x64::goBinary(BinaryNode *node) {
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
            case BinaryOperator::EQUAL:
                printf("\tcmp %%rdi, %%rax\n");
                printf("\tsete %%al\n");
                printf("\tmovzb %%al, %%rax\n");
                break;
            case BinaryOperator::NOT_EQUAL:
                printf("\tcmp %%rdi, %%rax\n");
                printf("\tsetne %%al\n");
                printf("\tmovzb %%al, %%rax\n");
                break;
            case BinaryOperator::GREATER:
                printf("\tcmp %%rdi, %%rax\n");
                printf("\tsetg %%al\n");
                printf("\tmovzb %%al, %%rax\n");
                break;
            case BinaryOperator::GREATER_OR_EQUAL:
                printf("\tcmp %%rdi, %%rax\n");
                printf("\tsetge %%al\n");
                printf("\tmovzb %%al, %%rax\n");
                break;
            case BinaryOperator::LESSER:
                printf("\tcmp %%rdi, %%rax\n");
                printf("\tsetl %%al\n");
                printf("\tmovzb %%al, %%rax\n");
                break;
            case BinaryOperator::LESSER_OR_EQUAL:
                printf("\tcmp %%rdi, %%rax\n");
                printf("\tsetle %%al\n");
                printf("\tmovzb %%al, %%rax\n");
                break;
            default:
                assert(0);
                break;
        }
    }

    void x64::goConstant(ConstantNode *node) {
        printf("\tmov $%d, %%rax\n", node->value);
    }

    void x64::Push() {
        printf("\tpush %%rax\n");
        this->stackLevel++;
    }

    void x64::Pop(const char *reg) {
        printf("\tpop %s\n", reg);
        this->stackLevel--;
    }

    void x64::goStatement(StatementNode *node) {
        node->left->Accept(this);
    }

    void x64::goAssign(AssignNode *node) {
        auto vNode = std::dynamic_pointer_cast<VariableNode>(node->left);
        assert(vNode != nullptr);
        printf("\tlea %d(%%rbp), %%rax\n", vNode->variableObject->offset);
        this->Push();
        node->right->Accept(this);
        this->Pop("%rdi");
        printf("\tmov %%rax, (%%rdi)\n");
    }

    void x64::goVariable(VariableNode *node) {
        printf("\tlea %d(%%rbp), %%rax\n", node->variableObject->offset);
        printf("\tmov (%%rax), %%rax\n");
    }
} // Front