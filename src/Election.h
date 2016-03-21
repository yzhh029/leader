//
// Created by yzhh0 on 3/10/2016.
//

#ifndef LEADER_ELECTION_H
#define LEADER_ELECTION_H

#include <vector>
#include "Host.h"
#include <string>
#include "HostManager.h"
#include "MessageQueue.h"
#include <atomic>
#include <queue>
#include <memory>


const int MAXBUFF = 1024;

class MessageQueue;

class Election {
public:
    Election(std::vector<Host> hlist, std::string p, int _self_id) ;

    ~Election();

    bool validateLeader(int timeout);
    bool elect();
    //* GetLeader() const { return leader; }
    void InitNet();
    bool isRunning() const { return running; }
    Message GetMessage();
    void run();

    friend void RecvLoop(Election* ele);
private:

    int view_number;

    HostManager host_group;
    int leader_id;
    int self_id;

    int sock;
    std::string port;

    std::atomic_bool running;

    std::shared_ptr<MessageQueue> recv_msgs;

};


#endif //LEADER_ELECTION_H
