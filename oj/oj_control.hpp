#pragma once
#include "../comm/httplib.h"
#include "oj_model.hpp"
#include "oj_view.hpp"
#include <algorithm> // std::sort
#include <jsoncpp/json/json.h>
#include <mutex>
namespace core_control
{
using std::string;
using namespace core_model;
using namespace core_view;
using std::endl;
using namespace httplib;
class Machine // 后续要对锁进行优化 ,目前的锁确实容易出现bug,在读取时候
{
public:
    Machine(string ip = nullptr, int port = 0, std::mutex* mtx = nullptr)
        : _ip(ip)
        , _port(port)
        , _mtx(mtx)
    {
    }
    ~Machine() {}
    string GetIp() { return _ip; }
    int GetPort() { return _port; }
    int GetBala()
    {
        std::lock_guard<std::mutex> lock(*_mtx);
        return _balance;
    }
    void incbala()
    {
        std::lock_guard<std::mutex> lock(*_mtx);
        _balance++;
    }
    void desbala()
    {
        std::lock_guard<std::mutex> lock(*_mtx);
        _balance--;
    }
    void reset()
    {
        std::lock_guard<std::mutex> lock(*_mtx);
        _balance = 0;
    }

private:
    string _ip;
    int _port;
    int _balance = 0;
    std::mutex* _mtx;
};

//加载主机配置
string configpath = "./service/machine.list";

//负载均衡模块
class LoadBalance
{
public:
    LoadBalance() { assert(LoadConfigFile(configpath)); }
    ~LoadBalance() {}
    bool LoadConfigFile(const string& config)
    {
        std::ifstream in(config);
        if (!in.is_open())
        {
            return false;
        }
        string line;
        while (getline(in, line))
        {
            vector<string> token;
            StringUtil::StringSplit(line, &token, ":");
            if (token.size() != 2)
            {
                continue;
            }
            Machine m(token[0], atoi(token[1].c_str()), new std::mutex);
            online.push_back(mac.size());
            mac.push_back(m);
        }
        Log(info) << "主机配置文件加载成功" << std::endl;
        in.close();
        return true;
    }
    bool SmartChoice(int* no, Machine** m)
    {
        std::lock_guard<std::mutex> lock(_mtx); //整体切换为RAII锁
        if (online.size() == 0)
        {
            Log(fatal) << "致命问题没有可用于的编译的服务器" << std::endl;
            return false;
        }
        int bala = mac[online[0]].GetBala();
        *no = online[0];
        *m = &mac[online[0]];
        for (auto& e : online)
        {

            if (bala > mac[e].GetBala())
            {
                *no = e;
                *m = &mac[e];
                bala = mac[e].GetBala();
            }
        }
        return true;
    }
    void SetOffline(int id)
    {
        std::lock_guard<std::mutex> lock(_mtx); //整体切换为RAII锁
        auto it = find(online.begin(), online.end(), id);
        if (it == online.end())
        {
            return;
        }
        online.erase(it);
        mac[id].reset();
        offline.push_back(id);
    }
    void SetOnline()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        online.insert(online.end(), offline.begin(), offline.end());
        offline.erase(offline.begin(), offline.end());
    }

private:
    vector<Machine> mac;
    vector<int> online;
    vector<int> offline;
    std::mutex _mtx;
};

class Control
{
public:
    Control() {}
    ~Control() {}
    void RestartService()
    {
        best_.SetOnline();
        Log(info) << "服务器全部上线" << endl;
    }
    bool AllQuestion(string* html)
    {
        vector<question> arr;
        bool flag = true;
        if (model_.GetAllQuestion(&arr))
        {
            std::sort(arr.begin(), arr.end(),
                      [](const question& q1, const question& q2) { return atoi(q1.id.c_str()) < atoi(q2.id.c_str()); });
            view_.AllAppendHtml(arr, html);
        }
        else
        {
            *html = "获取题目列表失败";
            flag = false;
        }
        return flag;
    }
    bool OneQuestion(const string& id, string* html)
    {
        question q;
        bool flag = true;
        if (model_.GetNumQusetion(id, &q))
        {
            view_.OneAppendHtml(q, html);
        }
        else
        {
            *html = "获取题目详情失败";
            flag = false;
        }
        return flag;
    }
    bool Judge(const string& id, const string& in_json, string* out)
    {
        question q;
        if (!model_.GetNumQusetion(id, &q))
        {
            Log(error) << "出现错误,拿不到题目" << endl;
        }
        //反序列化
        Json::Value in_value;
        Json::Reader reader;
        reader.parse(in_json, in_value);
        std::string code = in_value["code"].asString();
        code = code + "\n" + q.tail; //加个换行符 防止出现}#这种错误
        Json::Value out_value;
        out_value["code"] = code;
        out_value["input"] = "";
        out_value["cpu_limit"] = q.cpu_limit;
        out_value["mem_limit"] = q.mem_limit;

        //序列化

        //  std::string code = in_value["code"].asString();
        // std::string input = in_value["input"].asString();
        // int cpu_limit = in_value["cpu_limit"].asInt();
        // int mem_limit = in_value["mem_limit"].asInt(); 编译端需要的代码

        Json::FastWriter write;
        std::string outfile = write.write(out_value);

        //客户端传输
        while (true)
        {
            int id;
            Machine* m;
            if (!best_.SmartChoice(&id, &m))
            {
                Log(error) << "没有服务器" << endl;
                return false;
            }

            m->incbala();
            Client clt(m->GetIp(), m->GetPort());
            auto res = clt.Post("/compiler_and_run", outfile, "application/json");

            if (res)
            {
                //运行完减负载
                m->desbala();
                if (res->status == 200)
                {

                    *out = res->body;

                    Log(info) << "运行主机" << id << "运行详情" << m->GetIp() << "当前负载" << m->GetBala() << endl;
                    break;
                }
            }
            else
            {
                best_.SetOffline(id); //优化
                Log(info) << id << "当前服务器可能已经宕机了,请速速维修" << endl;
            }
        }
        return true;
    }

private:
    Model model_;
    View view_;
    LoadBalance best_;
};
} // namespace core_control