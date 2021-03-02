#include "turing.h"
using namespace std;

//public
int Simulator::run(string i)
{
    input = i;
    int wrongseq = 0;
    bool s = inputcheck(wrongseq);
    if (verbose)
        verboseInput(s, wrongseq);
    if (!s)
    {
        return err6;
    }
    /* 初始化纸带 */
    int size = tapenum - 1;
    if (input.empty())
    {
        input += blank;
    }
    tapes.push_back(input);
    heads.push_back(0);
    vector<int> firstindex;
    for (int i = 0; i < input.length(); i++)
    {
        firstindex.push_back(i);
    }
    indexs.push_back(firstindex);
    string b;
    b += blank;
    vector<int> otherindex;
    otherindex.push_back(0);
    for (int i = 0; i < size; i++)
    {
        tapes.push_back(b);
        heads.push_back(0);
        indexs.push_back(otherindex);
    }
    //printall();
    string TapeNow, StateNow;
    StateNow = start;
    TapeNow = getTapeNow();
    int seq = 0;
    while (1)
    {
        if (verbose)
        {
            verboseState(seq, StateNow);    //输出verbose模式下相关信息
        }
        if (finals.find(StateNow) != finals.end()) //到达终止态，运行结束
        {
            break;
        }
        From f = {StateNow, TapeNow};
        map<From, To>::iterator it = deltas.find(f); //读取delta函数
        if (it == deltas.end())                      //无相关转移函数，运行结束
        {
            break;
        }
        To t = it->second;
        TuringChange(t, StateNow, TapeNow);
        seq++;
    }
    FinalOutput();
    return 0;
}

//private
string Simulator::getTapeNow()
{
    string s;
    for (int i = 0; i < tapes.size(); i++)
    {
        s += tapes[i][heads[i]];
    }
    return s;
}

/*
    改变状态，包括更改：
    当前状态、当前纸带内容、索引、指针位置
*/
void Simulator::TuringChange(To t, string &StateNow, string &TapeNow)
{
    string statenext = t.statenext;
    string tapedir = t.tapedir;
    string tapewrite = t.tapewrite;
    string b;
    b += blank;
    //改变纸带内容
    for (int i = 0; i < tapenum; i++)
    {
        tapes[i][heads[i]] = tapewrite[i];
    }
    //移动下标
    for (int i = 0; i < tapenum; i++)
    {
        char dir = tapedir[i];
        if (dir == '*') //不动
        {
            continue;
        }
        else if (dir == 'l') //左移
        {
            if (heads[i] > 0)
                heads[i]--;
            else //heads[i]==0
            {
                if (tapes[i] == b) //空纸带，不添加blank，index改变
                {
                    indexs[i][0]--;
                }
                tapes[i] = blank + tapes[i];
                indexs[i].insert(indexs[i].begin(), *indexs[i].begin() - 1);
            }
        }
        else if (dir == 'r') //右移
        {
            if (heads[i] < tapes[i].length() - 1)
                heads[i]++;
            else //heads[i]==tapes[i].length() - 1
            {
                if (tapes[i] == b) //空纸带，不添加blank，index改变
                {
                    indexs[i][0]++;
                }
                tapes[i] = tapes[i] + blank;
                indexs[i].push_back(*(indexs[i].end() - 1) + 1);
                heads[i]++;
            }
        }
    }
    //赋新值
    StateNow = statenext;
    TapeNow = getTapeNow();
}

int width(int num) //获得数字宽度
{
    int n = 1;
    while (num = num / 10)
    {
        if (num == 0)
            break;
        n++;
    }
    return num;
}
string int2str(int val)
{
    ostringstream out;
    out << val;
    return out.str();
}

void Simulator::FinalOutput()
{
    string substr;
    int left = 0;
    for (; left < tapes[0].length(); left++)
    {
        if (tapes[0][left] != blank)
            break;
    }
    int right = tapes[0].length() - 1;
    if (left > right)
    {
        substr += blank;
    }
    else
    {
        for (; right > left; right--)
        {
            if (tapes[0][right] != blank)
                break;
        }
        substr = tapes[0].substr(left, right + 1);
    }

    if (!verbose)
    {
        cout << substr << endl;
    }
    else
    {
        int w = width(tapenum - 1); //获取长度，便于左对齐输出
        cout << std::left << std::setw(w + 7) << "Result";
        cout << ": " << substr << endl;
        cout << "==================== END ====================" << endl;
    }

    // for (int i = 0; i < tapenum; i++)
    //     cout << tapes[i] << endl;
}

void Simulator::verboseInput(bool s, int seq)
{
    cout << "Input: " << input << endl;
    if (s) //true
    {
        cout << "==================== RUN ====================" << endl;
    }
    else //false
    {
        cout << "==================== ERR ====================" << endl;
        cout << "error: '" << input[seq] << "' was not declared in the set of input symbols" << endl;
        cout << "Input: " << input << endl;
        cout << "       ";
        for (int i = 0; i < seq; i++)
            cout << ' ';
        cout << '^' << endl;
        cout << "==================== END ====================" << endl;
    }
}

void Simulator::verboseState(int step, string state)
{
    int w = width(tapenum - 1); //获取长度，便于左对齐输出
    cout << std::left << std::setw(w + 7) << "Step";
    cout << ": " << step << endl;
    for (int i = 0; i < tapenum; i++)
    {
        string index, indexContent, tape, tapeContent, head, headContent;
        index = "Index" + int2str(i);
        tape = "Tape" + int2str(i);
        head = "Head" + int2str(i);
        int left = 0;
        for (; left < tapes[i].length(); left++)
        {
            if (tapes[i][left] != blank || left == heads[i])
                break;
        }
        int right = tapes[i].length() - 1;
        for (; right > left; right--)
        {
            if (tapes[i][right] != blank || right == heads[i])
                break;
        }
        if (left > right) //全空
        {
            tapeContent += blank;
            headContent += '^';
            indexContent += int2str(abs(indexs[i][0]));
        }
        else
        {
            for (int j = left; j <= right; j++)
            {
                int idx = abs(indexs[i][j]);
                if (j < heads[i])
                    headContent += "  ";
                else if (j == heads[i])
                    headContent += '^';
                tapeContent += tapes[i][j];
                tapeContent += " ";
                indexContent += int2str(idx) + ' ';
                int indexw = width(idx);
                for (int k = 0; k < indexw - 1; k++) //左对齐
                {
                    tapeContent += ' ';
                    headContent += ' ';
                }
            }
        }
        cout << std::left << std::setw(w + 7) << index;
        cout << ": " << indexContent << endl;
        cout << std::left << std::setw(w + 7) << tape;
        cout << ": " << tapeContent << endl;
        cout << std::left << std::setw(w + 7) << head;
        cout << ": " << headContent << endl;
    }
    cout << std::left << std::setw(w + 7) << "State";
    cout << ": " << state << endl;
    cout << "---------------------------------------------" << endl;
}

bool Simulator::inputcheck(int &seq)
{
    for (int j = 0; j < input.length(); j++)
    {
        if (inputsymbols.find(input[j]) == inputsymbols.end())
        {
            seq = j;
            return false;
        }
    }
    return true;
}

// void Simulator::printall()
// {
//     cout << "states:" << endl;
//     for (set<string>::iterator it = states.begin(); it != states.end(); it++)
//     {
//         cout << *it << ' ';
//     }
//     cout << endl
//          << "---------------------------------------------" << endl;

//     cout << "input set:" << endl;
//     for (set<char>::iterator it = inputsymbols.begin(); it != inputsymbols.end(); it++)
//     {
//         cout << *it << ' ';
//     }
//     cout << endl
//          << "---------------------------------------------" << endl;

//     cout << "all set:" << endl;
//     for (set<char>::iterator it = tapesymbols.begin(); it != tapesymbols.end(); it++)
//     {
//         cout << *it << ' ';
//     }
//     cout << endl
//          << "---------------------------------------------" << endl;

//     cout << "start state:" << endl
//          << start << endl
//          << "---------------------------------------------" << endl;

//     cout << "final states:" << endl;
//     for (set<string>::iterator it = finals.begin(); it != finals.end(); it++)
//     {
//         cout << *it << ' ';
//     }
//     cout << endl
//          << "---------------------------------------------" << endl;

//     cout << "tapenum: " << tapenum << "; blank symbol: " << blank << endl
//          << "---------------------------------------------" << endl;

//     cout << "Tapes:" << endl;
//     for (vector<string>::iterator it = tapes.begin(); it != tapes.end(); it++)
//         cout << *it << endl;
//     cout << "---------------------------------------------" << endl;

//     cout << "Deltas:" << endl
//          << "Statenow Tapenow Tapewrite Tapedir Statenext" << endl;
//     for (map<From, To>::iterator it = deltas.begin(); it != deltas.end(); it++)
//     {
//         cout << it->first.statenow << ' ' << it->first.tapenow << ' '
//              << it->second.tapewrite << ' ' << it->second.tapedir << ' ' << it->second.statenext << endl;
//     }
//     cout << "---------------------------------------------" << endl;
// }