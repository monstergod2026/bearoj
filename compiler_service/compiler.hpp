#pragma once
#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using std::string;
namespace core_compiler
{
using namespace core_util;
using namespace core_log;
class Compiler
{
public:
    Compiler();
    ~Compiler();
    static bool CompileProgram(const string& compilefile)
    {
        pid_t pid = fork();
        if (pid < 0)
        {

            Log(error);
            exit(1);
        }
        else if (pid == 0)
        {
            umask(0);
            int comperr_ = open(PathUtil::GetComperrPatn(compilefile).c_str(), O_CREAT | O_RDWR, 0644);
            if (comperr_ < 0)
            {
                // todo
                Log(warning) << "std文件没有生成" << std::endl;
            }

            dup2(comperr_, 2); //输入到本地错误文件

            // g++ -o 目标文件 源文件 -std=c++11 重定向
            execlp("g++", "g++", "-o", PathUtil::GetExePatn(compilefile).c_str(),
                   PathUtil::GetSourcePatn(compilefile).c_str(), "-std=c++11", nullptr);

            Log(error) << "程序替换崩溃" << std::endl;
            exit(2);
        }
        else
        {
            waitpid(pid, nullptr, 0);

            if (FileUtil::IfFileExist(PathUtil::GetExePatn(compilefile)))
            {
                Log(info) << "程序正常输出" << std::endl;
                return true; //有文件
            }

            Log(fatal) << "没有编译出程序程序" << std::endl;
            return false; //没有文件
        }
    }
};
} // namespace core_compiler
