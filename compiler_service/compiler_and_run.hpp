#pragma once
#include "compiler.hpp"
#include "runservice.hpp"
#include <jsoncpp/json/json.h>
#include <string>
namespace core_compiler_and_run
{
using namespace core_compiler;
using namespace core_runservice;
class compiler_and_run
{
public:
    compiler_and_run();
    ~compiler_and_run();
    //清理文件
    static void RemoveAllTemp(const string& filename)
    {
        //删除可能生成的文件 .cpp .comperr .exe .stdin .stdout .stderr
        string _source = PathUtil::GetSourcePatn(filename);
        if (!FileUtil::IfFileExist(_source))
        {
            unlink(_source.c_str());
        }

        string _comperr = PathUtil::GetComperrPatn(filename);
        if (!FileUtil::IfFileExist(_comperr))
        {
            unlink(_comperr.c_str());
        }

        string _exe = PathUtil::GetExePatn(filename);
        if (!FileUtil::IfFileExist(_exe))
        {
            unlink(_exe.c_str());
        }

        string _stderr = PathUtil::GetStderrPath(filename);
        if (!FileUtil::IfFileExist(_stderr))
        {
            unlink(_stderr.c_str());
        }

        string _stdin = PathUtil::GetStdinPath(filename);
        if (!FileUtil::IfFileExist(_stdin))
        {
            unlink(_stdin.c_str());
        }

        string _stdout = PathUtil::GetStdoutPath(filename);
        if (!FileUtil::IfFileExist(_stdout))
        {
            unlink(_comperr.c_str());
        }
    }
    //状态码转为原因
    static string CodeToResult(int code, const string& filename)
    {
        string desc;

        switch (code)
        {
        case 0:
            desc = "编译运行成功";
            break;
        case -1:
            desc = "提交的代码是空";
            break;
        case -2:
            desc = "未知错误";
            break;
        case -3:
            // desc = "代码编译的时候发生了错误";
            FileUtil::ReadFile(PathUtil::GetComperrPatn(filename), &desc, true);
            break;
        case SIGABRT: // 6
            desc = "内存超过范围";
            break;
        case SIGXCPU: // 24
            desc = "CPU使用超时";
            break;
        case SIGFPE: // 8
            desc = "浮点数溢出";
            break;
        default:
            desc = "未知: " + std::to_string(code);
            break;
        }

        return desc;
    }
    static void Start(const string& in_json, string* out_json)
    {
        Json::Value in_value;
        Json::Reader reader;
        reader.parse(in_json, in_value); //最后在处理差错问题

        std::string code = in_value["code"].asString();
        std::string input = in_value["input"].asString();
        int cpu_limit = in_value["cpu_limit"].asInt();
        int mem_limit = in_value["mem_limit"].asInt();
        string filename;

        Json::Value out_value;
        int status_code = 0;
        int result_code = 0;
        if (code.size() == 0)
        {
            status_code = -1; //空代码
            goto END;
        }

        filename = FileUtil::UniqFileName();

        if (!FileUtil::WriteFile(PathUtil::GetSourcePatn(filename), code))
        {
            status_code = -2; //写文件失败
            goto END;
        }

        if (!Compiler::CompileProgram(filename))
        {
            status_code = -3; //编译出错
            goto END;
        }

        result_code = Runservice::GetRunStatue(filename, cpu_limit, mem_limit);
        if (result_code == 0)
        {
            //运行成功
            status_code = 0;
        }
        else if (result_code < 0)
        {
            status_code = -2; //运行出错
            goto END;
        }
        else
        {
            status_code = result_code; //运行异常
            goto END;
        }
    END:
        out_value["code"] = status_code;
        out_value["result"] = CodeToResult(status_code, filename);
        //如果run成功运行 则需要运行结果相关信息
        if (status_code == 0)
        {
            //输出运行结果
            string outstring;
            FileUtil::ReadFile(PathUtil::GetStdoutPath(filename), &outstring, true);
            out_value["stdout"] = outstring;

            //输出运行错误信息
            string errstring;
            FileUtil::ReadFile(PathUtil::GetStderrPath(filename), &errstring, true);
            out_value["stderr"] = errstring;
        }
        Json::StyledWriter writer;
        *out_json = writer.write(out_value);
        RemoveAllTemp(filename);
    }
};

} // namespace core_compiler_and_run