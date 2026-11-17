#pragma once
#include "oj_model.hpp"
#include "oj_view.hpp"
namespace core_control
{
using std::string;
using namespace core_model;
using namespace core_view;
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

private:
    Model model_;
    View view_;
};
} // namespace core_control