/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_PARSER_H
#define KNIFE_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include <unordered_map>

namespace Front {

    class Parser {
    private:
        Lexer& lexer;
        std::list<std::shared_ptr<Variable>>* variables {nullptr};
        std::unordered_map<std::string_view, std::shared_ptr<Variable>> variableMap;
    public:
        explicit Parser(Lexer& lexer);
        std::shared_ptr<ProgramNode> Parse();
    private:
        std::shared_ptr<ASTNode> Function();
        std::shared_ptr<ASTNode> FunctionCall();
        std::shared_ptr<ASTNode> Statement();
        std::shared_ptr<ASTNode> Assgin();
        std::shared_ptr<ASTNode> Equal();
        std::shared_ptr<ASTNode> Relational();
        std::shared_ptr<ASTNode> Expression();
        std::shared_ptr<ASTNode> Term();
        std::shared_ptr<ASTNode> Factor();
        std::shared_ptr<ASTNode> Primary();

        std::shared_ptr<Variable> FindVariable(std::string_view name);
        std::shared_ptr<Variable> MakeVariable(std::string_view name);
    };

} // Front

#endif //KNIFE_PARSER_H
