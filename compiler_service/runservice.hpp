#pragma once
#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include <fcntl.h>
#include <string>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
namespace core_runservice
{
using namespace core_util;
using namespace core_log;
class Runservice
{
    static void SetProcLimit(int _cpu_limit, int _mem_limit)
    {
        // 设置CPU时长
        struct rlimit cpu_rlimit;
        cpu_rlimit.rlim_max = RLIM_INFINITY;
        cpu_rlimit.rlim_cur = _cpu_limit;
        setrlimit(RLIMIT_CPU, &cpu_rlimit);

        // 设置内存大小
        struct rlimit mem_rlimit;
        mem_rlimit.rlim_max = RLIM_INFINITY;
        mem_rlimit.rlim_cur = _mem_limit * 1024; //转化成为KB
        setrlimit(RLIMIT_AS, &mem_rlimit);
    }

public:
    Runservice();
    ~Runservice();
    static int GetRunStatue(const std::string& compilefile, int cpu_limit, int mem_limit)
    {
        umask(0);
        //标注输入 标准输出 标准错误
        int _stdinfd = open(PathUtil::GetStdinPath(compilefile).c_str(), O_CREAT | O_RDONLY, 0644);
        int _stdoutfd = open(PathUtil::GetStdoutPath(compilefile).c_str(), O_CREAT | O_RDWR, 0644);
        int _stderrfd = open(PathUtil::GetStderrPath(compilefile).c_str(), O_CREAT | O_RDWR, 0644);
        if (_stdinfd < 0 || _stdoutfd < 0 || _stderrfd < 0)
        {
            Log(error) << "打开文件失败" << std::endl;
            return -1;
        }
        pid_t pid = fork();
        if (pid < 0)
        {
            close(_stdinfd);
            close(_stdoutfd);
            close(_stderrfd);
            Log(error) << "创建子进程失败" << std::endl;
            return -2;
        }
        else if (pid == 0)
        {
            dup2(_stdinfd, 0);
            dup2(_stdoutfd, 1);
            dup2(_stderrfd, 2);
            SetProcLimit(cpu_limit, mem_limit);
            execl(PathUtil::GetExePatn(compilefile).c_str(), PathUtil::GetExePatn(compilefile).c_str(), nullptr);
            Log(error) << "程序替换失败" << std::endl;
            exit(1);
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            close(_stdinfd);
            close(_stdoutfd);
            close(_stderrfd);
            Log(info) << "程序运行成功" << std::endl;
            std::cout << (status & 0x7f) << std::endl;
            return status & 0x7f;
        }
    }
};
} // namespace core_runservice