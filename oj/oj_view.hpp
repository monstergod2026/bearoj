#pragma once
#include "oj_model.hpp"
#include <ctemplate/template.h>
namespace core_view
{
using namespace core_model;
using std::string;
class View
{
public:
    View() {}
    ~View() {}
    void AllAppendHtml(const vector<question>& arr, string* out_html)
    {
        ctemplate::TemplateDictionary dict("test");
        for (const auto& e : arr)
        {
            ctemplate::TemplateDictionary* sub = dict.AddSectionDictionary("question_list");
            sub->SetValue("number", e.id);
            sub->SetValue("title", e.title);
            sub->SetValue("star", e.level);
        }
        //获取渲染对象
        ctemplate::Template* tp =
            ctemplate::Template::GetTemplate("./template/allquestion.html", ctemplate::DO_NOT_STRIP);
        //渲染
        tp->Expand(out_html, &dict);
    }
    void OneAppendHtml(const question& q, string* out_html)
    {
        ctemplate::TemplateDictionary dict("test");

        dict.SetValue("number", q.id);
        dict.SetValue("title", q.title);
        dict.SetValue("star", q.level);
        dict.SetValue("desc", q.desc);
        dict.SetValue("pre_code", q.header);

        ctemplate::Template* tp =
            ctemplate::Template::GetTemplate("./template/onequestion.html", ctemplate::DO_NOT_STRIP);
        tp->Expand(out_html, &dict);
    }
};
} // namespace core_view