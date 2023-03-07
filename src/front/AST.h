/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_AST_H
#define KNIFE_AST_H

#include <memory>
#include <list>

namespace Front {
    enum class BinaryOperator {
        ADD,
        SUB,
        MUL,
        DIV,
        EQUAL,
        NOT_EQUAL,
        GREATER,
        GREATER_OR_EQUAL,
        LESSER,
        LESSER_OR_EQUAL,
    };

    class Variable {
    public:
        std::string_view name;
        int offset;
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
        std::list<std::shared_ptr<ASTNode>> statements;
        std::list<std::shared_ptr<Variable>> variables;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Statement Node
     * 语句节点
     */
    class StatementNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> left {nullptr};

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
     * Variable Node
     * 变量节点
     */
    class VariableNode : public ASTNode {
    public:
        std::shared_ptr<Variable> variableObject;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Assign Node
     * 赋值节点
     */
    class AssignNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> left;
        std::shared_ptr<ASTNode> right;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Block Node
     * 代码块节点
     */
    class BlockNode : public ASTNode {
    public:
        std::list<std::shared_ptr<ASTNode>> statements;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Judgment Node
     * 判断节点
     */
    class IfNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> Condition {nullptr};
        std::shared_ptr<ASTNode> Then {nullptr};
        std::shared_ptr<ASTNode> Else {nullptr};

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * While Node
     * While循环节点
     */
    class WhileNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> Condition {nullptr};
        std::shared_ptr<ASTNode> Then {nullptr};
        std::shared_ptr<ASTNode> Else {nullptr};

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Node Visitor
     * 节点访问器
     */
    class NodeVisitor {
    public:
        virtual void goProgram(ProgramNode* node) {};
        virtual void goStatement(StatementNode* node) {};
        virtual void goBinary(BinaryNode* node) {};
        virtual void goConstant(ConstantNode* node) {};
        virtual void goVariable(VariableNode* node) {};
        virtual void goAssign(AssignNode* node) {};
        virtual void goBlock(BlockNode* node) {};
        virtual void goIf(IfNode* node) {};
        virtual void goWhile(WhileNode* node) {};
    };
}

#endif //KNIFE_AST_H
