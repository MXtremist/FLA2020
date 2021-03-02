#include "turing.h"
using namespace std;

bool CheckName(string str) //检查文件结尾是否为.tm
{
    int str_length = str.length();
    if (str_length <= 3)
        return false;
    if (str.compare(str_length - 3, 3, ".tm") == 0)
    {
        return true;
    }
    return false;
}
//public
Terminal::Terminal()
{
    myargc = 0;
    myargv = nullptr;
    verbose = false;
}

Terminal::Terminal(int argc, char **x)
{
    myargc = argc;
    myargv = x;
    verbose = false;
}

int Terminal::terminalcheck() //检查Terminal读入参数格式
{
    if (myargc < 2 || myargc > 4)
    {
        return err1;
    }
    else
    {
        string x1 = myargv[1];
        if ((x1 == "-h" || x1 == "--help") && myargc == 2) //help
        {
            return -1;
        }
        else if (CheckName(x1) && myargc == 3) //normal
        {
            input = myargv[2];
            filename = myargv[1];
            return 0;
        }
        else if ((x1 == "-v" || x1 == "--verbose") && CheckName(myargv[2]) && myargc == 4) //-v
        {

            input = myargv[3];
            verbose = true;
            filename = myargv[2];
            return 0;
        }
    }
    return err1;
}

int Terminal::run()
{
    int ans = terminalcheck();
    //cout << "Terminalcheck: " << ans << endl;
    if (ans == -1) //help
    {
        help();
        return 0;
    }
    if (ans == 0)
        ans = p.run(input, filename, verbose);
    if (ans != 0)
    {
        if (ans == err1)
            cerr << "command error" << endl;
        else if (ans == err2)
            cerr << "open file \"" << filename << "\" error" << endl;
        else if (ans >= err3 && ans <= err5)
            cerr << "syntax error" << endl;
        else if (ans == err6)
        {
            if (!verbose)
                cerr << "illegal input" << endl;
        }
        else
        {
            cerr << "error code " << ans << "not exist" << endl;
        }
    }
    return ans;
}

//private
void Terminal::help() //打印help信息
{
    cout << "usage: turing [-v|--verbose] [-h|--help] <tm> <input>" << endl;
}