#include "turing.h"
using namespace std;

bool setcheck(string &str)
{
    //检查是否是集合，如果是将{}删除
    if (str.length() > 1 && str[0] == '{' && str[str.length() - 1] == '}')
    {
        str = str.substr(1, str.length() - 2);
        return true;
    }
    return false;
}
bool stringcheck(string str)
{
    for (int i = 0; i < str.length(); i++)
    {
        char c = str[i];
        if (!((c > '0' || c < '9') || (c > 'a' || c < 'z') || (c > 'A' || c < 'Z') || (c == '_')))
        {
            return false;
        }
    }
    return true;
}
bool charcheckinput(char c)
{
    return ((c != ' ') && (c != ',') && (c != ';') && (c != '*') && (c != '_') && (c >= 32 && c <= 126));
}
bool charcheckall(char c)
{
    return ((c != ' ') && (c != ',') && (c != ';') && (c != '*') && (c >= 32 && c <= 126));
}
bool charcheck(char c, bool x)
{
    if (x)
        return charcheckall(c);
    else
        return charcheckinput(c);
}

bool splitwordsString(string s, string delim, set<string> &ret)
{
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != string::npos)
    {
        string str = s.substr(last, index - last);
        if ((ret.find(str) != ret.end()) || !stringcheck(str) || str.length() < 1)
        {
            return false;
        }
        ret.insert(str);
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0)
    {
        string str = s.substr(last, index - last);
        if ((ret.find(str) != ret.end()) || !stringcheck(str) || str.length() < 1)
        {
            return false;
        }
        ret.insert(str);
    }
    return true;
}
bool splitwordsChar(string s, string delim, set<char> &ret, bool x)
{
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != string::npos)
    {
        string str = s.substr(last, index - last);
        if (str.length() != 1 || (ret.find(str[0]) != ret.end()) || !charcheck(str[0], x))
        {
            return false;
        }
        ret.insert(str[0]);
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0)
    {
        string str = s.substr(last, index - last);
        if (str.length() != 1 || (ret.find(str[0]) != ret.end()) || !charcheck(str[0], x))
        {
            return false;
        }
        ret.insert(str[0]);
    }
    return true;
}

bool dircheck(string direct)
{
    for (int i = 0; i < direct.length(); i++)
    {
        if (direct[i] != 'r' && direct[i] != 'l' && direct[i] != '*')
            return false;
    }
    return true;
}

//public
Parser::Parser()
{
    verbose = false;
    input = '\0';
    filename = '\0';
}

int Parser::parsercheck()
{
    ifstream fin;
    string fpath = "../programs/" + filename;
    fin.open(fpath, ios::in);
    if (!fin)
    {
        return err2; //打开文件失败
    }
    string line;
    int ans = err3;
    int lseq = 0;
    while (getline(fin, line))
    {
        lseq++;
        if (line.length() < 1 || line[0] == ';')
            continue;
        int index = line.find_first_of(";"); //删除注释
        if (index != string::npos)
            line = line.substr(0, index);
        istringstream sin(line);
        string firstword;
        if (!(sin >> firstword))
            continue;
        if (firstword[0] == '#')
        {
            ans = hashcheck(line, firstword);
            if (ans == 0)
            {
                //为verbose模式存储相关信息
                if (firstword[1] == 'q')
                {
                    verboseseqs ss = {line, lseq};
                    hashseqs[0] = ss;
                }
                else if (firstword[1] == 'B')
                {
                    verboseseqs ss = {line, lseq};
                    hashseqs[1] = ss;
                }
                else if (firstword[1] == 'F')
                {
                    verboseseqs ss = {line, lseq};
                    hashseqs[2] = ss;
                }
            }
        }
        else
        {
            //delta函数共5个words
            int time = 1;
            vector<string> words;
            words.push_back(firstword);
            while (sin >> firstword)
            {
                words.push_back(firstword);
            }
            if (words.size() != 5)
            {
                ans = err3;
                break;
            }
            parserdelta d = {words[0], words[1], words[2], words[3], words[4], lseq};
            deltas.push_back(d);
            ans = 0;
        }
        if (ans != 0)
            break;
    }
    fin.close();
    if (verbose && ans == err3)
        verboseOutput3(line, lseq); //输出err3，即#行的图灵机格式错误
    if (ans != 0)
        return ans;
    string wrongword;
    ans = finalcheck(line, wrongword, lseq);
    if (verbose && (ans == err4 || ans == err5))
        verboseOutput45(line, wrongword, lseq, ans); //输出err4和5
    return ans;
}

int Parser::run(string i, string f, bool v)
{
    input = i;
    filename = f;
    verbose = v;
    int ans = parsercheck();
    //cout << "Parsercheck: " << ans << endl;
    if (ans != 0)
    {
        return ans;
    }
    initSimDelta(); //初始化simulator的转移函数
    ans = s.run(input);
    return ans;
}

void Parser::initSimDelta()
{
    for (vector<parserdelta>::iterator it = deltas.begin(); it != deltas.end(); it++)
    {
        From f = {it->statenow, it->tapenow};
        To t = {it->tapewrite, it->tapedir, it->statenext};
        s.deltas.insert(map<From, To>::value_type(f, t));
    }
    s.verbose = verbose;
}

//private
int Parser::hashcheck(string line, string firstword)
{
    if (line.length() - firstword.length() < 4)
        return err3;
    if (line[firstword.length()] != ' ' || line[firstword.length() + 1] != '=' || line[firstword.length() + 2] != ' ')
        return err3;
    int ans = err3;
    //只保留后半部分
    if (firstword == "#Q")
    {
        ans = Qcheck(line.substr(5, line.length() - 1));
    }
    else if (firstword == "#S")
    {
        ans = Scheck(line.substr(5, line.length() - 1));
    }
    else if (firstword == "#G")
    {
        ans = Gcheck(line.substr(5, line.length() - 1));
    }
    else if (firstword == "#q0")
    {
        ans = q0check(line.substr(6, line.length() - 1));
    }
    else if (firstword == "#B")
    {
        ans = Bcheck(line.substr(5, line.length() - 1));
    }
    else if (firstword == "#F")
    {
        ans = Fcheck(line.substr(5, line.length() - 1));
    }
    else if (firstword == "#N")
    {
        ans = Ncheck(line.substr(5, line.length() - 1));
    }
    return ans;
}
int Parser::Qcheck(string line)
{
    if (!setcheck(line))
        return err3;
    if (splitwordsString(line, ",", s.states))
        return 0;
    else
        return err3;
}
int Parser::Scheck(string line)
{
    if (!setcheck(line))
        return err3;
    if (splitwordsChar(line, ",", s.inputsymbols, false))
        return 0;
    else
        return err3;
}
int Parser::Gcheck(string line)
{
    if (!setcheck(line))
    {
        return err3;
    }
    if (splitwordsChar(line, ",", s.tapesymbols, true))
        return 0;
    else
        return err3;
}
int Parser::q0check(string line)
{
    if (line.length() < 1 || !(stringcheck(line)))
        return err3;
    s.start = line;
    return 0;
}
int Parser::Bcheck(string line)
{
    if (line.length() > 1 || !(charcheckall(line[0])))
        return err3;
    s.blank = line[0];
    return 0;
}
int Parser::Fcheck(string line)
{
    if (!setcheck(line))
        return err3;
    if (splitwordsString(line, ",", s.finals))
        return 0;
    else
        return err3;
}
int Parser::Ncheck(string line)
{
    int n = 0;
    for (int i = line.length() - 1; i > -1; i--)
    {
        char c = line[i];
        if (!(c > '0' || c < '9'))
            return err3;
        n = n * 10 + c - '0';
    }
    if (n <= 0)
        return err3;
    s.tapenum = n;
    return 0;
}

int Parser::finalcheck(string &line, string &wrongword, int &lseq)
{
    if (s.states.find(s.start) == s.states.end())
    {
        wrongword = s.start;
        line = hashseqs[0].line;
        lseq = hashseqs[0].seq;
        return err4;
    }
    if (s.tapesymbols.find(s.blank) == s.tapesymbols.end())
    {
        wrongword = s.blank;
        line = hashseqs[1].line;
        lseq = hashseqs[1].seq;
        return err4;
    }
    for (set<string>::iterator it = s.finals.begin(); it != s.finals.end(); it++)
    {
        if (s.states.find(*it) == s.states.end())
        {
            wrongword = *it;
            line = hashseqs[2].line;
            lseq = hashseqs[2].seq;
            return err4;
        }
    }
    //TODO:对deltas进行检查
    int ans = 0;
    vector<parserdelta>::iterator it = deltas.begin();
    int dflag = 0;
    for (; it != deltas.end(); it++)
    {
        if (s.states.find(it->statenow) == s.states.end())
            dflag = 1; //delta中statenow不存在
        else if (s.states.find(it->statenext) == s.states.end())
            dflag = 2; //delta中statenext不存在
        else if (dircheck(it->tapedir) == false)
            dflag = 3; //delta中方向存在无意义字符
        else if (it->tapedir.length() != s.tapenum)
            dflag = 4; //方向数量不匹配
        else if (it->tapenow.length() != s.tapenum)
            dflag = 5; //纸带数量不匹配
        else if (it->tapewrite.length() != s.tapenum)
            dflag = 6; //写入数量不匹配
        else
        {
            string tapenow = it->tapenow;
            string tapewrite = it->tapewrite;
            for (int i = 0; i < tapenow.length(); i++)
            {
                if (s.tapesymbols.find(tapenow[i]) == s.tapesymbols.end())
                {
                    dflag = 7; //tapenow中存在不在符号表中的
                    break;
                }
                if (s.tapesymbols.find(tapewrite[i]) == s.tapesymbols.end())
                {
                    dflag = 8; //tapewrite中存在不在符号表中的
                    break;
                }
            }
        }
        if (dflag != 0)
            break;
    }
    line = it->statenow + ' ' + it->tapenow + ' ' + it->tapewrite + ' ' + it->tapedir + ' ' + it->statenext;
    lseq = it->lseq;
    switch (dflag)
    {
    case 1:
        wrongword = it->statenow;
        ans = err4;
        break;
    case 2:
        wrongword = it->statenext;
        ans = err4;
        break;
    case 3:
        wrongword = it->tapedir;
        ans = err4;
        break;
    case 4:
        wrongword = it->tapedir;
        ans = err5;
        break;
    case 5:
        wrongword = it->tapenow;
        ans = err5;
        break;
    case 6:
        wrongword = it->tapewrite;
        ans = err5;
        break;
    case 7:
        wrongword = it->tapenow;
        ans = err4;
        break;
    case 8:
        wrongword = it->tapewrite;
        ans = err4;
        break;
    default:
        ans = 0;
        break;
    }
    return ans;
}

void Parser::verboseOutput3(string line, int lseq)
{
    cerr << "Something wrong in file \"" << filename << "\", line " << lseq << ", \"" << line << "\", formal error" << endl;
}

void Parser::verboseOutput45(string line, string wrongword, int lseq, int err)
{
    if (err == 4)
    {
        cerr << "Something wrong in file \"" << filename << "\", line " << lseq << ", \"" << line << "\", \"" << wrongword << "\" is not exist" << endl;
    }
    if (err == 5)
    {
        cerr << "Something wrong in file \"" << filename << "\", line " << lseq << ", \"" << line << "\", \"" << wrongword << "\" is not matched with tape num" << endl;
    }
}