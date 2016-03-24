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
    Election(std::vector<Host> hlist, std::string p, int _self_id, std::string _self_name, int quorum) ;

    ~Election();

    bool validateLeader(int timeout);
    bool Propose(std::string value);
    int elect();
    //* GetLeader() const { return leader; }
    void InitNet();
    bool isRunning() const { return running; }
    Message GetMessage();
    void run();

    friend void RecvLoop(Election* ele);

private:
    int GetEpochSec();
    int NewProposalNum();
    void NextView(int);
    //int GetProposalNum(int cli_id, int view);

    // election protocol
    int mini_qourum;
    HostManager host_group;
    std::atomic_int leader_id;
    std::atomic_int last_live; // last heartbeat time
    std::atomic_int accepted_proposal;
    std::atomic_int min_proposal;
    std::atomic_int my_proposal;

    // self information
    int self_id;
    int prop_number;
    int view_number;
    std::string self_name;
    
    // network
    int sock;
    std::string port;

    std::atomic_bool running;

    std::shared_ptr<MessageQueue> normal_msgs;
    std::shared_ptr<MessageQueue> vote_message;

};


#endif //LEADER_ELECTION_H
