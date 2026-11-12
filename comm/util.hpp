#pragma once
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
using std::string;
namespace core_util
{
class PathUtil
{
    static string GetFilePath(const string& compilefile, const char* filetype)
    {
        string path = "./Temp/";
        path += compilefile;
        path += filetype;
        return path;
    }

public:
    //编译输出
    static string GetSourcePatn(const string& compilefile) { return GetFilePath(compilefile, ".cpp"); }
    static string GetExePatn(const string& compilefile) { return GetFilePath(compilefile, ".exe"); }
    static string GetComperrPatn(const string& compilefile) { return GetFilePath(compilefile, ".comperr"); }
    //运行输出
    static string GetStdinPath(const string& compilefile) { return GetFilePath(compilefile, ".stdin"); }
    static string GetStdoutPath(const string& compilefile) { return GetFilePath(compilefile, ".stdout"); }
    static string GetStderrPath(const string& compilefile) { return GetFilePath(compilefile, ".stderr"); }
};

class FileUtil
{
public:
    static bool IfFileExist(const string& compilefile)
    {

        struct stat st;
        int _id = stat(PathUtil::GetExePatn(compilefile).c_str(), &st);
        if (_id == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    static string UniqFileName() { return "code"; }                      //测试用
    static void WriteFile(const string& compilefile, const string& code) //测试用
    {
        int _fd = open(PathUtil::GetSourcePatn(compilefile).c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
        ssize_t n = write(_fd, code.c_str(), code.size());
        close(_fd);
    }
};
class TimeUtil
{
public:
    static string GetTimeStamp()
    {
        struct timeval _time;
        gettimeofday(&_time, nullptr);
        return std::to_string(_time.tv_sec);
    }
};

} // namespace core_util