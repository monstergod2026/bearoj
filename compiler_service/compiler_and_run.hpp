#pragma once
#include "compiler.hpp"
#include "runservice.hpp"
#include <jsoncpp/json/json.h>
namespace core_compiler_and_run
{
using namespace core_compiler;
using namespace core_runservice;
class compiler_and_run
{
public:
    compiler_and_run();
    ~compiler_and_run();
    static void Start(const string& in_json, string* out_json)
    {
        Json::Value in_value;
        Json::Reader reader;
        reader.parse(in_json, in_value); //最后在处理差错问题

        std::string code = in_value["code"].asString();
        std::string input = in_value["input"].asString();
        int cpu_limit = in_value["cpu_limit"].asInt();
        int mem_limit = in_value["mem_limit"].asInt();

        if (code.size() == 0)
        {
            return;
        }

        string filename = FileUtil::UniqFileName();
        FileUtil::WriteFile(filename, code);
        Compiler::CompileProgram(filename);
        Runservice::GetRunStatue(filename, cpu_limit, mem_limit);
    }
};

} // namespace core_compiler_and_run