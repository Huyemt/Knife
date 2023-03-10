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

        for (auto &f : node->Functions) {
            f->Accept(this);
        }
    }

    void x64::goFunction(FunctionNode *node) {
        this->CurrentFunctionName = node->Name;
        printf(".globl %s\n", this->CurrentFunctionName.data());
        printf("%s:\n", this->CurrentFunctionName.data());

        /*
         * Space allocation
         * 内存空间分配
         */
        int stackSize = 0;
        for (auto &v : node->Variables) {
            stackSize += 8;
            v->Offset = -stackSize;
        }

        stackSize = this->AlignTo(stackSize, 16);

        printf("\tpush %%rbp\n");
        printf("\tmov %%rsp, %%rbp\n");
        printf("\tsub $%d, %%rsp\n", stackSize);

        for (int i = 0; i < node->Params.size(); ++i) {
            printf("\tmov %s, %d(%%rbp)\n", RegX64[i], node->Params[i]->Offset);
        }

        for (auto &s : node->Statements) {
            s->Accept(this);
            assert(this->stackLevel == 0);
        }

        printf(".FunctionReturn_%s:\n", this->CurrentFunctionName.data());
        printf("\tmov %%rbp, %%rsp\n");
        printf("\tpop %%rbp\n");
        printf("\tret\n");
    }

    void x64::goFunctionReturn(FunctionReturnNode *node) {
        node->Left->Accept(this);
        printf("\tjmp .FunctionReturn_%s\n", this->CurrentFunctionName.data());
    }

    void x64::goFunctionCall(FunctionCallNode *node) {
        for (auto &p : node->Params) {
            p->Accept(this);
            this->Push();
        }
        for (int i = node->Params.size() - 1; i >= 0; i--) {
            this->Pop(this->RegX64[i]);
        }

        std::string name = std::string(node->Name);
        printf("\tcall %s\n", name.data());
    }

    void x64::goBinary(BinaryNode *node) {
        node->Right->Accept(this);
        this->Push();
        node->Left->Accept(this);
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
        printf("\tmov $%d, %%rax\n", node->Value);
    }

    void x64::Push() {
        printf("\tpush %%rax\n");
        this->stackLevel++;
    }

    void x64::Pop(const char *reg) {
        printf("\tpop %s\n", reg);
        this->stackLevel--;
    }

    int x64::AlignTo(int size, int align) {
        return (size + align - 1) / align * align;
    }

    void x64::goStatement(StatementNode *node) {
        if (node->Left) {
            node->Left->Accept(this);
        }
    }

    void x64::goAssign(AssignNode *node) {
        auto vNode = std::dynamic_pointer_cast<VariableNode>(node->Left);
        assert(vNode != nullptr);
        printf("\tlea %d(%%rbp), %%rax\n", vNode->VariableObject->Offset);
        this->Push();
        node->Right->Accept(this);
        this->Pop("%rdi");
        printf("\tmov %%rax, (%%rdi)\n");
    }

    void x64::goVariable(VariableNode *node) {
        printf("\tlea %d(%%rbp), %%rax\n", node->VariableObject->Offset);
        printf("\tmov (%%rax), %%rax\n");
    }

    void x64::goIf(IfNode *node) {
        int seq = this->IfSequence++;
        node->Condition->Accept(this);
        printf("\tcmp $0, %%rax\n");

        if (node->Else) {
            printf("\tje .Logic.else_%d\n", seq);
        } else {
            printf("\tje .Logic.end_%d\n", seq);
        }

        node->Then->Accept(this);
        printf("\tjmp .Logic.end_%d\n", seq);

        if (node->Else) {
            printf(".Logic.else_%d:\n", seq);
            node->Else->Accept(this);
        }

        printf(".Logic.end_%d:\n", seq);
    }

    void x64::goBlock(BlockNode *node) {
        for (auto &s : node->Statements) {
            s->Accept(this);
        }
    }

    void x64::goWhile(WhileNode *node) {
        int seq = this->LoopSequence++;
        printf(".Loop.begin_%d:\n", seq);

        node->Condition->Accept(this);

        printf("\tcmp $0, %%rax\n");
        printf("\tje .Loop.end_%d\n", seq);

        node->Then->Accept(this);

        printf("\tjmp .Loop.begin_%d\n", seq);
        printf(".Loop.end_%d:\n", seq);

        if (node->Else) {
            node->Else->Accept(this);
        }
    }

    void x64::goFor(ForNode *node) {
        int seq = this->LoopSequence++;
        if (node->Init) {
            node->Init->Accept(this);
        }

        printf(".Loop.begin_%d:\n", seq);

        if (node->Condition) {
            node->Condition->Accept(this);
            printf("\tcmp $0, %%rax\n");
            printf("\tje .Loop.end_%d\n", seq);
        }

        node->Then->Accept(this);

        if (node->Operate) {
            node->Operate->Accept(this);
        }

        printf("\tjmp .Loop.begin_%d\n", seq);
        printf(".Loop.end_%d:\n", seq);

        if (node->Else) {
            node->Else->Accept(this);
        }
    }
} // Front

