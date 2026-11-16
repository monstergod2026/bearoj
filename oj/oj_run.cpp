#include "../comm/httplib.h"
#include <iostream>
#include <string>
using namespace httplib;
int main()
{
    Server server;
    server.Get("/all_question",
               [](const Request& req, Response& res) { res.set_content("这是全部题目", "text/plain;charset=utf-8"); });
    server.Get(R"(/question/(\d+))",
               [](const Request& req, Response& res)
               {
                   std::string id = req.matches[1].str();
                   std::string message = "这是第" + id + "题";
                   res.set_content(message, "text/plain;charset=utf-8");
               });

    server.Get(R"(/judge/(\d+))",
               [](const Request& req, Response& res)
               {
                   std::string id = req.matches[1].str();
                   std::string message = "这是第" + id + "题的判断结果";
                   res.set_content(message, "text/plain;charset=utf-8");
               });

    server.listen("0.0.0.0", 8080);
    return 0;
}