/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#ifndef KNIFE_PARSER_H
#define KNIFE_PARSER_H

#include "Lexer.h"
#include "AST.h"

namespace Front {

    class Parser {
    private:
        Lexer& lexer;
    public:
        Parser(Lexer& lexer);
        std::shared_ptr<ProgramNode> Parse();
    private:
        std::shared_ptr<ASTNode> Expression();
        std::shared_ptr<ASTNode> Term();
        std::shared_ptr<ASTNode> Factor();
        std::shared_ptr<ASTNode> Primary();
    };

} // Front

#endif //KNIFE_PARSER_H
