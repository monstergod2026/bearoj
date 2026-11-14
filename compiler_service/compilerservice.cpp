#include "../comm/httplib.h"
#include "compiler_and_run.hpp"
using namespace core_compiler_and_run;
using namespace httplib;
void Usage()
{
    std::cout << "参数不对";
}
int main(int argc, char* argv[])
{
    if (argc != 2)
    {

        Usage();
        return -1;
    }

    Server server;

    server.Post("/compiler_and_run",
                [](const Request& req, Response& res)
                {
                    string in_json = req.body;
                    string out_json;

                    compiler_and_run::Start(in_json, &out_json);
                    res.set_content(out_json, "application/json;charset=utf-8");
                });
    server.listen("0.0.0.0", atoi(argv[1]));
    // string in_json =
    //     R"({"code":"#include <iostream>\nint main(){std::cout << \"hello bearOJ\" << std::endl;return
    //     0;}","input":"","cpu_limit":1,"mem_limit":32768})";
    // string out_json;

    // compiler_and_run::Start(in_json, &out_json); //测试

    // std::cout << out_json << std::endl;
    return 0;
}