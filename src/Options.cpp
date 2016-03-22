//
// Created by yzhh0 on 3/9/2016.
//

#include "Options.h"
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include <fstream>

using namespace std;

Options::Options(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        //cout << argv[i] << endl;
        if (strcmp(argv[i], "-p") == 0) {
            port = atoi(argv[++i]);
            cout << "port " << port << endl;
        } else if (strcmp(argv[i], "-h") == 0) {
            host_file = argv[++i];
            cout << "host file" << host_file << endl;
        } else if (strcmp(argv[i], "-f") == 0) {
            max_crash = atoi(argv[++i]);
            cout << "max crash" << max_crash << endl;
        } else {
            cout << "error flag " << argv[i] << endl;
            break;
        }
    }
}


std::vector<Host> Options::GetHosts() {
    ifstream hfile(host_file);
    vector<Host> hosts;
    string h;
    int id = 1;

    char _selfname[1024];
    gethostname(_selfname, 1024);
    selfname = string(_selfname);

    while (getline(hfile, h)) {
        if (h.empty())
            break;
        if (h != selfname)
            hosts.emplace_back(h, id++, GetPortStr());
        else {
            selfid = id++;
            cout << "self id " << selfid << " " << selfname << endl;
        }

    }

    return hosts;
}
