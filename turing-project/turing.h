#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <vector>
#include <set>
#include <map>
#include <iomanip>
using namespace std;

#define err1 1 //err1:命令行格式错误
#define err2 2 //err2:打开文件失败
#define err3 3 //err3:#行的图灵机格式错误
#define err4 4 //err4:出现不存在的状态或字符
#define err5 5 //err5:符号与纸带数不匹配
#define err6 6 //err6:输入不合法

//parser记录的转移函数
struct parserdelta
{
    string statenow;  //当前状态
    string tapenow;   //当前位置
    string tapewrite; //写入符号
    string tapedir;   //移动方向
    string statenext; //下一状态
    int lseq;         //文件行号
};
//parser用于verbose模式输出
struct verboseseqs
{
    string line;
    int seq;
};

//对于确定性图灵机，对每个From有且仅有一个To
struct From
{
    string statenow;
    string tapenow;
    //重载操作符，以便使用map
    bool operator<(const From &other) const
    {
        if (statenow != other.statenow)
        {
            return statenow < other.statenow;
        }
        else
        {
            return tapenow < other.tapenow;
        }
    }
};
struct To
{
    string tapewrite;
    string tapedir;
    string statenext;
};


class Simulator
{
private:
    //void printall();
    string getTapeNow();
    bool inputcheck(int &seq);
    void verboseState(int step, string state);
    void verboseInput(bool s, int seq);
    void TuringChange(To t, string &StateNow, string &TapeNow);
    void FinalOutput();

public:
    set<string> states;
    set<char> inputsymbols;
    set<char> tapesymbols;
    string start;
    set<string> finals;
    int tapenum;
    char blank;
    vector<string> tapes;
    vector<int> heads;
    vector<vector<int>> indexs;
    map<From, To> deltas;
    int run(string i);
    bool verbose;
    string input;
};

class Parser
{
private:
    string input;
    string filename;
    Simulator s;
    bool verbose;
    int hashcheck(string line, string firstword); //检查#行错误
    int Qcheck(string line);
    int Scheck(string line);
    int Gcheck(string line);
    int q0check(string line);
    int Bcheck(string line);
    int Fcheck(string line);
    int Ncheck(string line);
    int finalcheck(string &line, string &wrongword, int &lseq); //检查是否有err4或err5
    vector<parserdelta> deltas;
    verboseseqs hashseqs[3];
    void initSimDelta();

public:
    Parser();
    int parsercheck();
    int run(string i, string f, bool v);
    void verboseOutput3(string line, int lseq);
    void verboseOutput45(string line, string wrongword, int lseq, int err);
};

class Terminal
{
private:
    int myargc;
    char **myargv;
    void help();
    bool verbose;
    Parser p;
    string input;
    string filename;

public:
    Terminal();
    Terminal(int argc, char **x);
    int run();
    int terminalcheck();
};