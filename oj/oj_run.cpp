#include "../comm/httplib.h"
#include "oj_control.hpp"
#include <iostream>
#include <string>
using namespace httplib;
using namespace core_control;
using std::string;
int main()
{
    Server server;
    Control ctrl;
    server.Get("/all_questions",
               [&ctrl](const Request& req, Response& res)
               {
                   string message;
                   ctrl.AllQuestion(&message);
                   res.set_content(message, "text/html;charset=utf-8");
               });
    server.Get(R"(/question/(\d+))",
               [&ctrl](const Request& req, Response& res)
               {
                   std::string id = req.matches[1].str();
                   std::string message;
                   ctrl.OneQuestion(id, &message);
                   res.set_content(message, "text/html;charset=utf-8");
               });

    server.Post(R"(/judge/(\d+))",
                [&ctrl](const Request& req, Response& res)
                {
                    std::string id = req.matches[1].str();
                    std::string message = req.body;
                    std::string ans;
                    ctrl.Judge(id, message, &ans);
                    res.set_content(ans, "application/json;charset=utf-8");
                });
    server.set_base_dir("./wwwroot");
    server.listen("0.0.0.0", 8080);
    return 0;
}