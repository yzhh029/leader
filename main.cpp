#include <iostream>

#include "src/Host.h"
#include "src/Options.h"
#include "src/Election.h"


using namespace std;

int main(int argc, char* argv[]) {

    Options opt(argc, argv);

    cout << "port:      " << opt.GetPort() << endl
         << "host file: " << opt.GetHostFile() << endl
         << "max crash: " << opt.GetMaxCrash() << endl;

    vector<Host> hostlist = opt.GetHosts();
    for (auto &h : hostlist) {
        cout << h.GetId() << " " << h.GetHostName() << endl;
    }

    Election ele(hostlist, opt.GetPortStr(), opt.GetSelfId(), opt.GetSelfName(), hostlist.size() + 1 - opt.GetMaxCrash());

    ele.InitNet();

    ele.run();

    return 0;
}