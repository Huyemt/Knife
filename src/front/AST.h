/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_AST_H
#define KNIFE_AST_H

#include <memory>
#include <list>
#include <vector>

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
        std::string_view Name;
        int Offset;
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
        std::list<std::shared_ptr<ASTNode>> Functions;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Statement Node
     * 语句节点
     */
    class StatementNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> Left {nullptr};

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Binary Node
     * 二元节点
     */
    class BinaryNode : public ASTNode {
    public:
        BinaryOperator anOperator;
        std::shared_ptr<ASTNode> Left;
        std::shared_ptr<ASTNode> Right;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Constant Node
     * 常量节点
     */
    class ConstantNode : public ASTNode {
    public:
        int Value {0};

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Variable Node
     * 变量节点
     */
    class VariableNode : public ASTNode {
    public:
        std::shared_ptr<Variable> VariableObject;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Assign Node
     * 赋值节点
     */
    class AssignNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> Left;
        std::shared_ptr<ASTNode> Right;

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Block Node
     * 代码块节点
     */
    class BlockNode : public ASTNode {
    public:
        std::list<std::shared_ptr<ASTNode>> Statements;

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
     * For Node
     * For循环节点
     */
    class ForNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> Init {nullptr};
        std::shared_ptr<ASTNode> Condition {nullptr};
        std::shared_ptr<ASTNode> Operate {nullptr};
        std::shared_ptr<ASTNode> Then {nullptr};
        std::shared_ptr<ASTNode> Else {nullptr};

        void Accept(NodeVisitor* visitor) override;
    };

    /**
     * Function Node
     * 函数声明节点
     */
    class FunctionNode : public ASTNode {
    public:
        std::string_view Name;
        std::vector<std::shared_ptr<Variable>> Params {};
        std::list<std::shared_ptr<Variable>> Variables {};
        std::list<std::shared_ptr<ASTNode>> Statements {};

        void Accept(NodeVisitor* visitor) override;
    };

    class FunctionReturnNode : public ASTNode {
    public:
        std::shared_ptr<ASTNode> Left;

        void Accept(NodeVisitor *visitor) override;
    };

    /**
     * Function Call Node
     * 函数调用节点
     */
    class FunctionCallNode : public ASTNode {
    public:
        std::string_view Name;
        std::vector<std::shared_ptr<ASTNode>> Params;

        void Accept(NodeVisitor *visitor) override;
    };

    /**
     * Node Visitor
     * 节点访问器
     */
    class NodeVisitor {
    public:
        virtual void goProgram(ProgramNode* node) = 0;
        virtual void goStatement(StatementNode* node) = 0;
        virtual void goBinary(BinaryNode* node) = 0;
        virtual void goConstant(ConstantNode* node) = 0;
        virtual void goVariable(VariableNode* node) = 0;
        virtual void goAssign(AssignNode* node) = 0;
        virtual void goBlock(BlockNode* node) = 0;
        virtual void goIf(IfNode* node) = 0;
        virtual void goWhile(WhileNode* node) = 0;
        virtual void goFor(ForNode* node) = 0;
        virtual void goFunction(FunctionNode* node) = 0;
        virtual void goFunctionReturn(FunctionReturnNode* node) = 0;
        virtual void goFunctionCall(FunctionCallNode* node) = 0;
    };
}

#endif //KNIFE_AST_H
