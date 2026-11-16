#pragma once
#include <atomic>
#include <boost/algorithm/string.hpp>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
using std::string;
namespace core_util
{
using std::vector;
class TimeUtil
{
public:
    //获取时间戳
    static string GetTimeStamp()
    {
        struct timeval _time;
        gettimeofday(&_time, nullptr);
        return std::to_string(_time.tv_sec);
    }

    //获取毫秒级时间戳
    static string GetTimeMs()
    {
        struct timeval _time;
        gettimeofday(&_time, nullptr);
        return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
    }
};
class PathUtil
{

    //获取具体路径
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
    //判断文件是否存在
    static bool IfFileExist(const string& filename)
    {

        struct stat st;
        int _id = stat(filename.c_str(), &st);
        if (_id == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    //获取唯一文件名
    static string UniqFileName()
    {
        static std::atomic<int> num(0);
        num++;
        return "bear" + TimeUtil::GetTimeMs() + "_" + to_string(num);
    }

    //写文件
    static bool WriteFile(const string& compilefile, const string& code)
    {
        std::ofstream fout(compilefile);
        if (!fout.is_open())
        {
            return false;
        }
        fout.write(code.data(), code.size());
        fout.close();
        return true;
    }

    //读文件
    static bool ReadFile(const string& compilefile, string* out, bool keep)
    {
        out->clear();
        std::ifstream fin(compilefile);
        if (!fin.is_open())
        {
            return false;
        }
        std::string message;

        // getline 不保留换行符
        while (getline(fin, message))
        {
            (*out) += message;
            (*out) += keep ? "\n" : "";
        }
        return true;
    }
};
class StringUtil
{
public:
    static void StringSplit(const string& mes, vector<string>* token, string split)
    {
        boost::split(*token, mes, boost::is_any_of(split), boost::token_compress_on);
    }
};

} // namespace core_util