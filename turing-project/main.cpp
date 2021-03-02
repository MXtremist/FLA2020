#include "turing.h"
using namespace std;

int main(int argc, char *argv[])
{
    //char **x = argv;
    Terminal t(argc, argv);
    int ans = t.run();
    //cout << ans << endl;
    return ans == 0 ? 0 : -1;
}
