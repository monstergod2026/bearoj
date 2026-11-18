#pragma once
#include "../comm/httplib.h"
#include "oj_model.hpp"
#include "oj_view.hpp"
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
    int GetBala() { return _balance; }
    void incbala()
    {
        _mtx->lock();
        _balance++;
        _mtx->unlock();
    }
    void desbala()
    {
        _mtx->lock();
        _balance--;
        _mtx->unlock();
    }

private:
    string _ip;
    int _port;
    int _balance = 0;
    std::mutex* _mtx;
};

string configpath = "./service/machine.list";
class SmartChoice
{
public:
    SmartChoice() { assert(LoadConfigFile(configpath)); }
    ~SmartChoice() {}
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
            Machine m(token[0], atoi(token[1].c_str()), new std::mutex);
            online.push_back(mac.size());
            mac.push_back(m);
        }
        Log(info) << "主机配置文件加载成功" << std::endl;
        in.close();
        return true;
    }
    bool Loadbalance(int* no, Machine** m)
    {
        if (online.size() == 0)
        {
            Log(fatal) << "致命问题没有可用于的编译的服务器" << std::endl;
            return false;
        }
        _mtx.lock();
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
        _mtx.unlock();
        return true;
    }
    void SetOffline(int id)
    {
        _mtx.lock();
        auto it = online.begin();
        while (it != online.end())
        {
            if (*it == id)
            {
                online.erase(it);
                break;
            }
            it++;
        }
        offline.push_back(id);
        _mtx.unlock();
    }
    void SetOnline() {}

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
    bool AllQuestion(string* html)
    {
        vector<question> arr;
        bool flag = true;
        if (model_.GetAllQuestion(&arr))
        {
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
        code += q.tail;
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
            if (!best_.Loadbalance(&id, &m))
            {
                Log(error) << "没有服务器" << endl;
                return false;
            }

            m->incbala();
            Client clt(m->GetIp(), m->GetPort());
            auto res = clt.Post("/compiler_and_run", outfile, "application/json");
            if (res)
            {
                if (res->status == 200)
                {

                    *out = res->body;
                    m->desbala();
                    Log(info) << "运行ip" << m->GetIp() << endl;
                    break;
                }
            }
            else
            {
                best_.SetOffline(id); //优化
            }
        }
        return true;
    }

private:
    Model model_;
    View view_;
    SmartChoice best_;
};
} // namespace core_control