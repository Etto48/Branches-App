//
// Created by the_E on 20/05/28.
//

#include "pch.h"

#include "../algebraParser/algebraParser.h"
#include "exprNode.h"


using namespace std;

///exprNode
exprNode::exprNode(string expr)
{
    //cout<<"Found expr:"<<expr<<endl;
    left = right = nullptr;

    original = expr;

    algebra_tools_::removeWrappingBraces(expr);

    int depth = 0;
    unsigned fmeo = -1;
    bool found = false;
    for (auto &pop: algebraParser::ops())
    {
        for (int j = int(expr.size()) - 1; j >= 0; j--)///find first most extern operator
        {
            auto &c = expr[j];
            //cout<<"C:"<<c<<endl;
            if (c == ')')depth += 1;
            else if (c == '(')depth -= 1;
            else if (depth == 0 && pop.find(c) != string::npos)
            {
                //cout<<"found operator "<<c<<" in "<<expr<<endl;
                fmeo = j;

                bool isError=false;
                if(fmeo>0)///check if detected a false positive
                {
                    auto tmp = algebraParser::ops();
                    unsigned k = 0;
                    for (; k < tmp.size(); k++)
                        if (tmp[k] == pop)
                            break;
                    if (k >= tmp.size() - 1)
                        break;
                    vector<std::string>::const_iterator first = tmp.begin() + k;
                    vector<std::string>::const_iterator last = tmp.end();
                    std::vector<std::string> morePriority(first, last);
                    for (const auto &mp:morePriority)
                    {
                        if (expr[fmeo - 1] == mp[0])
                        {
                            isError=true;
                            break;
                        }
                    }
                }
                if(!isError)
                {
                    found = true;
                    break;
                }
            }
        }
        if (found)
            break;
    }
    if (fmeo <= 0 || fmeo >= expr.length() - 1)
        throw algebra_tools_::except("exprNode: Invalid Operator Location at "+to_string(fmeo)+" in "+expr);
    string sleft, sright;


    //super wrong

    sleft = expr.substr(0, fmeo);
    sright = expr.substr(fmeo + 1, expr.length() - fmeo - 1);
    //cout<<"L: "<<left<<" R: "<<right<<endl;

    op = expr.substr(fmeo, 1);

    if (op == "/" && sright == "0")
        throw algebra_tools_::except("Divide by 0");

    left = algebra_tools_::newAdequateNode(sleft);
    right = algebra_tools_::newAdequateNode(sright);
}

T exprNode::compile(map<string, T> &symMap)
{
    if (op == "+")
        return left->compile(symMap) + right->compile(symMap);
    if (op == "-")
        return left->compile(symMap) - right->compile(symMap);
    if (op == "*")
        return left->compile(symMap) * right->compile(symMap);
    if (op == "/")
        return left->compile(symMap) / right->compile(symMap);
    if (op == "^")
        return pow(left->compile(symMap), right->compile(symMap));
    else
        throw algebra_tools_::except("exprNode: Invalid Operator Symbol");
}

std::string exprNode::derivative(const string &direction)
{
    if (op == "+")
    {
        auto ld = left->derivative(direction);
        auto rd = right->derivative(direction);
        if (ld == "0" && rd == "0")
            return "0";
        else if (ld == "0")
            return "(" + rd + ")";
        else if (rd == "0")
            return "(" + ld + ")";
        else
            return "(" + ld + ")+(" + rd + ")";
    } else if (op == "-")
    {
        auto ld = left->derivative(direction);
        auto rd = right->derivative(direction);
        if (ld == "0" && rd == "0")
            return "0";
        else if (ld == "0")
            return "-(" + rd + ")";
        else if (rd == "0")
            return "(" + ld + ")";
        else
            return "(" + ld + ")-(" + rd + ")";
    } else if (op == "*")
    {
        auto ld = left->derivative(direction);
        auto rd = right->derivative(direction);
        auto l = left->original;
        auto r = right->original;

        if (ld != "0" && rd != "0" && r != "0" && l != "0")
            return "(" + ld + ")*(" + r + ")+(" + l + ")*(" + rd + ")";
        else if ((ld == "0" || r == "0") && l != "0" && rd != "0")
            return "(" + l + ")*(" + rd + ")";
        else if ((l == "0" || rd == "0") && ld != "0" && r != "0")
            return "(" + ld + ")*(" + r + ")";
        else
            return "0";
    } else if (op == "/")
    {
        auto ld = left->derivative(direction);
        auto rd = right->derivative(direction);
        auto l = left->original;
        auto r = right->original;

        if ((ld == "0" && rd == "0") || (l == "0" && r != "0"))
            return "0";
        else if (ld != "0" && rd != "0" && r != "0" && l != "0")
            return "((" + ld + ")/(" + r + ")-(" + l + ")*(" + rd + ")/((" + r + ")^2))";
        else if ((ld == "0") && l != "0" && rd != "0")
            return "-((" + l + ")*(" + rd + ")/((" + r + ")^2))";
        else if ((l == "0" || rd == "0") && ld != "0" && r != "0")
            return "((" + ld + ")/(" + r + "))";
        else
            throw algebra_tools_::except("Divide by 0");
    } else if (op == "^")
    {
        auto ld = left->derivative(direction);
        auto rd = right->derivative(direction);
        auto l = left->original;
        auto r = right->original;

        if (rd == "0" && ld == "0")
            return "0";
        else if (rd == "0")
            return "((" + r + ")*(" + l + ")^((" + r + ")-1))*(" + ld + ")";
        else if (ld == "0")
            return "((" + l + ")^(" + r + ")*((" + rd + ")*ln(" + l + ")))";
        else
            return "((" + l + ")^(" + r + ")*((" + rd + ")*ln(" + l + ")+((" + r + ")*(" + ld + "))/(" + l + ")))";
    } else
        throw algebra_tools_::except("Invalid operator");
}

std::string exprNode::simplify()
{
    string retl, retr;
    try
    {
        T v = left->compile();
        if (v < 0)
            retl = "-(" + algebra_tools_::dtos(-v) + ")";
        else
            retl = algebra_tools_::dtos(v);
    } catch (algebra_tools_::except &)
    {
        retl = left->simplify();
        algebra_tools_::removeWrappingBraces(retl);
    }
    try
    {
        T v = right->compile();
        if (v < 0)
            retr = "-(" + algebra_tools_::dtos(-v) + ")";
        else
            retr = algebra_tools_::dtos(v);
    } catch (algebra_tools_::except &)
    {
        retr = right->simplify();
        algebra_tools_::removeWrappingBraces(retl);
    }

    //cout<<"L: "<<retl<<" R: "<<retr<<endl;
    ///simplify optimizations wip
    if (op == "+" && (retr == "-(" + retl + ")" ||
                      (algebra_tools_::strEqD(retl, 0) && algebra_tools_::strEqD(retr, 0))))//if sides are inverse or 0
        return "0";
    else if (op == "+" && (retl == retr))
        return (left->nodetype() == "expr" && retl.size() != 1) ? "2*(" + retl + ")" : "2*" + retl;
    else if (op == "+" && algebra_tools_::strEqD(retl, 0) && !algebra_tools_::strEqD(retr, 0))
        return ((right->nodetype() == "expr" && retr.size() != 1) ? "(" + retr + ")" : retr);
    else if (op == "+" && !algebra_tools_::strEqD(retl, 0) && algebra_tools_::strEqD(retr, 0))
        return ((left->nodetype() == "expr" && retl.size() != 1) ? "(" + retl + ")" : retl);

    if (op == "-" && retl == retr)//if both sides are equal
        return "0";
    else if (op == "-" && (retr == "-(" + retl + ")"))
        return (left->nodetype() == "expr" && retl.size() != 1) ? "2*(" + retl + ")" : "2*" + retl;
    else if (op == "-" && (retl == "-(" + retr + ")"))
        return "-2*(" + retr + ")";
    else if (op == "-" && algebra_tools_::strEqD(retl, 0) && !algebra_tools_::strEqD(retr, 0))
        return "-(" + retr + ")";
    else if (op == "-" && !algebra_tools_::strEqD(retl, 0) && algebra_tools_::strEqD(retr, 0))
        return ((left->nodetype() == "expr" && retl.size() != 1) ? "(" + retl + ")" : retl);

    if (op == "*" && (retr == "-(" + retl + ")" ||
                      (algebra_tools_::strEqD(retl, 0) && algebra_tools_::strEqD(retr, 0))))//if sides are inverse or 0
        return (left->nodetype() == "expr" && retl.size() != 1) ? "-((" + retl + ")^2)" : "-(" + retl + "^2)";
    else if (op == "*" && (retl == retr))
        return (right->nodetype() == "expr" && retr.size() != 1) ? "(" + retl + ")^2" : retl + "^2";
    else if (op == "*" && algebra_tools_::strEqD(retl, 0) || algebra_tools_::strEqD(retr, 0))
        return "0";
    else if (op == "*" && algebra_tools_::strEqD(retl, 1) && !algebra_tools_::strEqD(retr, 1))
        return ((right->nodetype() == "expr" && retr.size() != 1) ? "(" + retr + ")" : retr);
    else if (op == "*" && !algebra_tools_::strEqD(retl, 1) && algebra_tools_::strEqD(retr, 1))
        return ((left->nodetype() == "expr" && retl.size() != 1) ? "(" + retl + ")" : retl);

    if (op == "/" && algebra_tools_::strEqD(retl, 0))
        return "0";
    else if (op == "/" && algebra_tools_::strEqD(retl, 1) && algebra_tools_::strEqD(retr, 1))
        return "1";
    else if (op == "/" && algebra_tools_::strEqD(retr, 1))
        return ((left->nodetype() == "expr" && retl.size() != 1) ? "(" + retl + ")" : retl);

    if (op == "^" && !algebra_tools_::strEqD(retl, 0) && algebra_tools_::strEqD(retr, 0))
        return "1";
    else if (op == "^" && algebra_tools_::strEqD(retr, 1))
        return ((left->nodetype() == "expr" && retl.size() != 1) ? "(" + retl + ")" : retl);

    else
    {
        if ((left->nodetype() == "expr" && retl.size() != 1) && (right->nodetype() == "expr" && retr.size() != 1))
            return "(" + retl + ")" + op + "(" + retr + ")";
        else if (left->nodetype() == "expr" && retl.size() != 1)
            return "(" + retl + ")" + op + retr;
        else if (right->nodetype() == "expr" && retr.size() != 1)
            return retl + op + "(" + retr + ")";
        else
            return retl + op + retr;
    }
}

std::string exprNode::nodetype()
{
    return "expr";
}

std::vector<std::string> exprNode::getVars()
{
    auto ret = left->getVars();
    auto ret2 = right->getVars();
    ret.insert(ret.end(), ret2.begin(), ret2.end());
    return ret;
}

exprNode::~exprNode()
{
    delete left;
    delete right;
}
