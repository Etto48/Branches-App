//
// Created by the_E on 20/06/02.
//

#include "pch.h"

#include "vectorParser.h"
#include <sstream>

using namespace std;

vectorParser::vectorParser(std::string str)
{

    ///expected input (f1(...),f2(...),...,fn(...))

    //expected input {<f(x,y,z)>,<g(x,y,z)>,...}
    //if (str[0] != '{' || str[str.length() - 1] != '}')//dimensions =1
    //    throw algebra_tools_::except("Wrong vector format");

    //remove braces
    //str = str.substr(1, str.length() - 2);

    algebra_tools_::removeWrappingBraces(str);

    stringstream ss(str);
    string item;
    vector<string> sStrings;

    while (getline(ss, item, ','))
    {
        sStrings.push_back(item);
    }

    for (auto &s:sStrings)
    {
        comps.emplace_back(s);
    }

}

vector<string> vectorParser::getVars()
{
    vector<string> ret;
    for (auto &c:comps)
    {
        auto ret2 = c.getVars();
        ret.insert(ret.end(), ret2.begin(), ret2.end());
    }
    return ret;
}

unsigned vectorParser::getDimensions()
{
    return (unsigned)comps.size();
}

std::string vectorParser::simplify()
{
    std::vector<std::string> ev;
    for (auto& ap : comps)
    {
        ev.push_back(ap.simplify());
    }

    std::string ret;
    if (getDimensions() != 1)
        ret += "(";
    unsigned i = 1;
    for (const auto& x : ev)
    {
        ret += x;
        if (i < getDimensions())
            ret += ",";
        i++;
    }
    if (getDimensions() != 1)
        ret += ")";
    return ret;
}

std::string vectorParser::evalToString(const std::map<std::string, T>& args)
{
    DT ev = evaluate(args);
    std::string ret;
    if (getDimensions() != 1)
        ret += "(";
    unsigned i = 1;
    for (const auto& x : ev.get())
    {
        ret += to_string(x);
        if (i < getDimensions())
            ret += ",";
        i++;
    }
    if (getDimensions() != 1)
        ret += ")";
    return ret;
}

DT vectorParser::evaluate(const map<string, T> &args)
{
    DT ret(getDimensions());
    unsigned i = 0;
    for (auto &p:comps)
    {
        ret[i] = p.evaluate(args);
        i++;
    }
    return ret;
}
