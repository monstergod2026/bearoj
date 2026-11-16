#pragma once
#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
namespace core_model
{
using namespace core_util;
using namespace core_log;
using std::string;
using std::vector;
struct question
{
    std::string id;
    std::string title;
    std::string level;
    int cpu_limit;
    int mem_limit;
    std::string desc;
    std::string header;
    std::string tail;
};
const std::string questionlist = "./question/question.list";
const std::string questionpath = "./question/";
class Model
{
public:
    Model() { assert(QuestionsCreate(questionlist)); }
    bool QuestionsCreate(const std::string list)
    {
        std::ifstream in(list);
        if (!in.is_open())
        {
            return false;
        }
        string message;
        while (getline(in, message))
        {
            vector<string> token;
            StringUtil::StringSplit(message, &token, " ");
            if (token.size() != 5)
            {
                continue;
            }
            struct question q;

            q.id = token[0];
            q.title = token[1];
            q.level = token[2];
            q.cpu_limit = atoi(token[3].c_str());
            q.mem_limit = atoi(token[4].c_str());
            string path = questionpath + q.id;
            path += "/";
            FileUtil::ReadFile(path + "dessc.txt", &q.desc, true);
            FileUtil::ReadFile(path + "header.cpp", &q.header, true);
            FileUtil::ReadFile(path + "tail.cpp", &q.tail, true);
        }
    }
    ~Model();
    bool GetAllQuestion(vector<question>& v)
    {
        if (questions.empty())
        {
            return false;
        }
        for (const auto& e : questions)
        {
            v.push_back(e.second);
        }
        return true;
    }
    bool GetNumQusetion(string id, struct question* ques)
    {
        auto it = questions.find(id);
        if (it == questions.end())
        {
            return false;
        }
        *ques = it->second;
    }
    bool GetAnswer() {}

private:
    std::unordered_map<std::string, question> questions;
};
} // namespace core_model