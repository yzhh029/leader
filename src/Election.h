//
// Created by yzhh0 on 3/10/2016.
//

#ifndef LEADER_ELECTION_H
#define LEADER_ELECTION_H

#include <vector>
#include "Host.h"
#include <string>


const int MAXBUFF = 1024;

struct Message {
    std::string msg;
    unsigned int addr;
};

class Election {
public:
    Election(std::vector<Host> hlist, std::string p) ;

    ~Election();

    bool validateLeader(int timeout);
    bool elect();
    Host* GetLeader() const { return leader; }
    void InitNet();
    Message GetMessage();
private:

    //Host* findHost()
    //Message GetMessage();

    std::vector<Host> host_group;
    Host* leader;
    Host* self;

    int sock;
    std::string port;

};


#endif //LEADER_ELECTION_H
