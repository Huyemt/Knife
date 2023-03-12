/*
 * Author: Huyemt
 * Time: 2023/3/11
 */

#ifndef KNIFE_TYPE_H
#define KNIFE_TYPE_H

#include "Lexer.h"
#include "AST.h"
#include <memory>
#include <list>
#include <utility>

namespace Front {
    class BuiltIn;
    class FunctionReturn;

    class Type {
    public:
        enum class TypeClass {
            BUILTIN,
            FUNCTION
        };

        static std::shared_ptr<BuiltIn> INT;
    private:
        int Size;
        int Align;
        TypeClass DataType;
    public:
        Type(TypeClass type, int size, int align) : DataType(type), Size(size), Align(align) {};
        virtual ~Type() {};

        [[nodiscard]] bool IsInteger() const;
        [[nodiscard]] bool IsFunction() const;
    };

    class BuiltIn : public Type {
    public:
        enum class DType {
            INT
        };
    private:
        DType dType;
    public:
        BuiltIn(DType dType, int size, int align) : Type(TypeClass::BUILTIN, size, align), dType(dType) {};

        [[nodiscard]] DType GetDataType() const;
    };

    struct Param {
        std::shared_ptr<Type> Base;
        std::shared_ptr<Token> Token;
    };

    class FunctionReturn : public Type {
    private:
        std::shared_ptr<Type> ReturnType;
    public:
        std::list<std::shared_ptr<Param>> Params;

        explicit FunctionReturn(std::shared_ptr<Type> base) : Type(TypeClass::FUNCTION, 8, 8), ReturnType(std::move(base)) {};
    };

    class TypeVisitor : public NodeVisitor {
    public:
        void goProgram(ProgramNode *node) override;
        void goStatement(StatementNode *node) override;
        void goStatementExperssion(StatementExperssionNode *node) override;
        void goUnary(UnaryNode *node) override;
        void goBinary(BinaryNode *node) override;
        void goConstant(ConstantNode *node) override;
        void goVariable(VariableNode *node) override;
        void goAssign(AssignNode *node) override;
        void goDeclaration(DeclarationNode *node) override;
        void goBlock(BlockNode *node) override;
        void goIf(IfNode *node) override;
        void goWhile(WhileNode *node) override;
        void goFor(ForNode *node) override;
        void goFunction(FunctionNode *node) override;
        void goFunctionReturn(FunctionReturnNode *node) override;
        void goFunctionCall(FunctionCallNode *node) override;
    };
} // Front

#endif //KNIFE_TYPE_H
