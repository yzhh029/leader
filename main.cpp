#include <iostream>

#include "src/Host.h"
#include "src/Options.h"
#include "src/Election.h"

using namespace std;

int main(int argc, char* argv[]) {

    Options opt(argc, argv);

    cout << "Hello, World!" << endl;
    cout << opt.GetPort() << " " << opt.GetHostFile() << " " << opt.GetMaxCrash() << endl;

    vector<Host> hostlist = opt.GetHosts();
    for (auto h : hostlist) {
        cout << h.GetId() << " " << h.GetHostName() << endl;
    }

    Election ele(hostlist, opt.GetPortStr());

    for (auto h : hostlist) {
        cout << h.GetId() << " " << h.GetHostName() << endl;
    }

    cout << " send hello to " << hostlist[1].GetHostName() << endl;
    hostlist[0].InitNet();
    hostlist[0].SendMessage("hello");

    ele.InitNet();
    ele.GetMessage();

    return 0;
}