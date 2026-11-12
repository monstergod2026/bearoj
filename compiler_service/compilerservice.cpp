#include "compiler_and_run.hpp"
using namespace core_compiler_and_run;
int main()
{
    string in_json =
        R"({"code":"#include <iostream>\nint main(){std::cout << \"hello bearOJ\" << std::endl;return 0;}","input":"","cpu_limit":1,"mem_limit":32768})";
    string out_json;

    compiler_and_run::Start(in_json, &out_json); //测试

    std::cout << out_json << std::endl;
    return 0;
}