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
    for (auto &h : hostlist) {
        cout << h.GetId() << " " << h.GetHostName() << endl;
    }

    Election ele(hostlist, opt.GetPortStr(), opt.GetSelfId(), opt.GetSelfName(), hostlist.size() + 1 - opt.GetMaxCrash());

    for (auto &h : hostlist) {
        cout << h.GetId() << " " << h.GetHostName() << endl;
    }

    ele.InitNet();

    ele.run();

    return 0;
}