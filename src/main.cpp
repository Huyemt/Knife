/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "front/Parser.h"
#include "front/generator/x64/Assembly.h"
#include <iostream>

using namespace Front;

/**
 * 翻译为汇编语言并且编译为可执行文件
 * @param source
 */
void compileAsm(std::string_view source) {
    Lexer lexer(source.data());

    Parser parser(lexer);
    auto ast = parser.Parse();

    Assembly generator;
    ast->Accept(&generator);
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << R"(Please input code -> .\knife "expression")" << std::endl;
        return 0;
    }
    compileAsm(argv[1]);
    return 0;

}