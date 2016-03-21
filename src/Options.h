//
// Created by yzhh0 on 3/9/2016.
//

#ifndef LEADER_OPTIONS_H
#define LEADER_OPTIONS_H

#include <string>
#include <vector>

#include "Host.h"
class Options {

public:
    explicit Options(int argc, char* argv[]);

    int GetPort() const { return port; }
    std::string GetPortStr() const { return std::to_string(port); }
    std::string GetHostFile() const {return host_file; }
    int GetMaxCrash() const { return max_crash; }
    int GetSelfId() const {return selfid;}

    std::vector<Host> GetHosts();
private:
    int port;
    std::string host_file;
    int max_crash;
    int selfid;
};


#endif //LEADER_OPTIONS_H
