/*
 * Author: Huyemt
 * Time: 2023/2/27
 */

#include "front/Parser.h"
#include "front/generator/assembly/x64.h"
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

    x64 generator;
    ast->Accept(&generator);
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << R"(Please input code -> .\knife "expression")" << std::endl;
        return 0;
    }

    FILE* fp = fopen(argv[1], "r");
    if (fp == nullptr) {
        std::cout << "This file is not exist: " << argv[1] << std::endl;
        exit(0);
    }

    char buffer[1024 * 10];
    size_t length = fread(buffer, 1, sizeof(buffer), fp);
    buffer[length] = '\0';

    compileAsm(buffer);
    return 0;

}