#pragma once
#include <iostream>
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
    static string GetSourcePatn(const string& compilefile) { return GetFilePath(compilefile, ".cpp"); }
    static string GetExePatn(const string& compilefile) { return GetFilePath(compilefile, ".exe"); }
    static string GetStderrPatn(const string& compilefile) { return GetFilePath(compilefile, ".stderr"); }
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