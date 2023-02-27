/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_AST_H
#define KNIFE_AST_H

#include <memory>

namespace Front {
    enum class BinaryOperator {
        ADD,
        SUB,
        MUL,
        DIV
    };
    class NodeVisitor;

    /**
     * Abstract Syntax Tree Node
     * 语法树节点
     */
    class ASTNode {
    public:
        virtual void Accept(NodeVisitor* visitor) {};
    };

    /**
     * Program Node
     * 程序节点 (主树根)
     */
    class ProgramNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> left;
        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Binary Node
     * 二元节点
     */
    class BinaryNode : public ASTNode {
    public:
        BinaryOperator anOperator;
        std::shared_ptr<ASTNode> left;
        std::shared_ptr<ASTNode> right;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Constant Node
     * 常量节点
     */
    class ConstantNode : public ASTNode {
    public:
        int value{0};
        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Node Visitor
     * 节点访问器
     */
    class NodeVisitor {
    public:
        virtual void goProgram(ProgramNode* node) {};
        virtual void goBinary(BinaryNode* node) {};
        virtual void goConstant(ConstantNode* node) {};
    };
}

#endif //KNIFE_AST_H
