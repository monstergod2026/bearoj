#include "compiler.hpp"
int main()
{
    string filename = "code";
    core_compiler::Compiler::CompileProgram(filename);
    return 0;
}