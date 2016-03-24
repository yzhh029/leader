//
// Created by yzhh0 on 3/10/2016.
//

#include "Election.h"
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <arpa/inet.h>
#include <map>
#include "utils.h"


using namespace std;


int Election::GetEpochSec() {
    return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
}

int Election::NewProposalNum() {
    return view_number * 100 + self_id;
}

/*
int Election::GetProposalNum(int cli_id, int view) {
    return view * 100 + cli_id;
}
*/

Election::Election(std::vector<Host> hlist, string p, int _self_id, string _self_name, int quorum)
        : host_group(HostManager(hlist)), leader_id(-1), self_id(_self_id), port(p),
          mini_qourum(quorum), view_number(1), self_name(_self_name), min_proposal(0),
          my_proposal(0), accepted_proposal(-1) {

    char hostname[64];
    if (gethostname(hostname, 64) == -1) {
        perror("get hostname faild");
    }

    else {
     cout << hostname << endl;
    }

    last_live = GetEpochSec();


    normal_msgs = make_shared<MessageQueue>();
    vote_message = make_shared<MessageQueue>();
}

void Election::InitNet() {

    host_group.InitAllNet();

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("server socket error");
        exit(1);
    }

    int enable =1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof enable) < 0) {
        perror("resue sock addr failed");
        close(sock);
        exit(1);
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(static_cast<uint16_t>(stoi(port)));

    int ret = bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1) {
        perror("bind error");
        close(sock);
        exit(1);
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        perror("set sock timeout error");
        exit(1);
    }

    cout << "initialized for server " << port << endl;
}

Message Election::GetMessage() {

    struct sockaddr_in client_addr;
    char buf[MAXBUFF];
    socklen_t addr_len;

    int size = recvfrom(sock, buf, MAXBUFF-1, 0, (struct sockaddr *)&client_addr, &addr_len);
    if (size < 0) {
        if (errno == EWOULDBLOCK) {
            //cout << "no message yet " << endl;
            return Message();
        } else {
            perror("receive from error");
            exit(1);
        }
    }

    Message msg(string(buf, size));
    //cout << " new recv " << msg.cli_id << " " << msg.view_num << " " << msg.msg << " " << msg.value << endl;
    int cli_addr = client_addr.sin_addr.s_addr;

    msg.srcHost = host_group.FindHostByAddr(cli_addr);
    if (msg.srcHost == nullptr) {
        msg.srcHost = host_group.FindHostById(msg.cli_id);
    }

    if (msg.srcHost == nullptr)
    {
        cout << " no host found " << msg.msg << endl;
    }
    return msg;
}

Election::~Election() {
    close(sock);
}


void RecvLoop(Election* ele) {

    cout << "recv start" << endl;
    while (ele->isRunning()) {
        Message msg = ele->GetMessage();

        if (msg.srcHost) {
            // receive heartbeat req, reply LIVE
            if (msg.msg.find("HEARTBEAT") != string::npos) {
                //msg.srcHost->SendMessage(Message(ele->self_id, msg.view_num, "LIVE"));
                if (ele->self_id == ele->leader_id) {
                    Print(ele->self_name, "recv heartbeat from " + msg.srcHost->GetHostName());
                    msg.srcHost->SendMessage(to_string(ele->self_id) + " " + to_string(msg.view_num) + " LIVE 0");
                }
                else
                    msg.srcHost->SendMessage(to_string(ele->self_id) + " " + to_string(msg.view_num) + " WRONGLEADER 0" );
            } else if (msg.msg.find("LIVE") != string::npos) {
                ele->last_live = ele->GetEpochSec();
                Print(ele->self_name, " leader " + msg.srcHost->GetHostName() + " is live");
                ele->host_group.HostIsLive(msg.cli_id);
            } else if (msg.msg.find("PREPARE") != string::npos) {
                if (msg.view_num > ele->min_proposal) {
                    cout << " PREPOK new min_proposal " << msg.view_num << endl;
                    ele->min_proposal = msg.view_num;
                }
                msg.srcHost->SendMessage(to_string(ele->self_id) + " " + to_string(ele->accepted_proposal) + " PREPOK " + to_string(ele->leader_id) );
                cout << " PREPOK to " << msg.srcHost->GetHostName() << ": last acpt view " << ele->accepted_proposal << " leader " << ele->leader_id << endl;
            } else if ( msg.msg.find("PREPOK") != string::npos || msg.msg.find("ACPTOK") != string::npos) {
                ele->vote_message->push(msg);
            } else if ( msg.msg.find("ACCEPT") != string::npos ) {
                if (msg.view_num >= ele->min_proposal) {
                    cout << " ACPTOK accept min_proposal " << msg.view_num << endl;
                    ele->accepted_proposal = ele->min_proposal = msg.view_num;
                    ele->leader_id = stoi(msg.value);
                }
                msg.srcHost->SendMessage(to_string(ele->self_id) + " " + to_string(ele->min_proposal) + " ACPTOK 0");
                cout << " ACPTOK to " << msg.srcHost->GetHostName() << ": acpt view " << ele->accepted_proposal << " leader " << ele->leader_id << endl;
            } else if (msg.msg.find("WRONGLEADER") != string::npos) {
                ele->leader_id = -1;
            } else {
                cout << "recv wrong message " << msg.msg << endl;
                ele->normal_msgs->push(msg);
            }
            //msg.srcHost->SendMessage(msg.msg + " ACK");

        }
    }
    cout << "recv end" << endl;
}


bool Election::Propose(std::string value) {

    //host_group.Boardcast();
}



int Election::elect() {

    // boardcast 1st-phase prepare to all other
    ++view_number;
    my_proposal = NewProposalNum();

    Print(self_name, "start new election quorum " + to_string(mini_qourum) + " view " + to_string(view_number));
    host_group.Boardcast(Message(self_id, my_proposal, "PREPARE"));

    int collect = 1, candidate_id = self_id;

    const auto TIMEOUT = chrono::seconds(2);
    auto start = chrono::system_clock::now();
    int highest_view = -1;

    // collect PREPOK from majority
    while (collect <= host_group.GetGroupSize()) {

        Message vote;

        if (vote_message->wait_and_pop(vote, 1000) && vote.msg.find("PREPOK") != string::npos) {
            cout << "a vote from " << vote.srcHost->GetHostName() << " view " << vote.view_num << " id " << vote.value << endl;
            // assume PREPOK meg
            if (vote.view_num > highest_view && stoi(vote.value) != -1) {
                cout << " update candidate from " << candidate_id << " to " << vote.value << endl;
                highest_view = vote.view_num;
                candidate_id = stoi(vote.value);
            }
            ++collect;
        }

        auto now = chrono::system_clock::now();
        if (now > start + TIMEOUT || collect >= mini_qourum) {
            break;
        }
    }
    cout << "stop voting recv " << collect << " votes" << endl;

    if (collect < mini_qourum){
        Print(self_name, "election failed " + to_string(collect) + "!=" + to_string(host_group.GetGroupSize() + 1));
        return -1;
    }
    /*else {
        Print(self_name, "enough vote, candidate: " + to_string(candidate_id));
        return candidate_id;
    }*/

    // 2nd phase

    host_group.Boardcast(Message(self_id, my_proposal, "ACCEPT", to_string(candidate_id)));
    collect = 1;
    start = chrono::system_clock::now();
    while (collect <= host_group.GetGroupSize()) {
        Message vote;

        if (vote_message->wait_and_pop(vote, 1000) && vote.msg.find("ACPTOK") != string::npos) {

            if (vote.view_num > my_proposal) {
                Print(self_name, "election fail my proposal " + to_string(my_proposal) + " max " + vote.value);
                return -1;
            }
            ++collect;
        }

        auto now = chrono::system_clock::now();
        if (now > start + TIMEOUT || collect >= mini_qourum) {
            break;
        }
    }
    if (collect < mini_qourum) {
        Print(self_name, "election fail no enough vote " + to_string(collect) + "<" + to_string(min_proposal));
        return -1;
    } else {
        Print(self_name, "election success " + to_string(my_proposal) + " new leader " + to_string(candidate_id));
        return candidate_id;
    }

}


void Election::run() {

    running = true;

    thread recv_thread(RecvLoop, this);
    Message msg;

    while (true) {

        if (leader_id == -1 ||  // if there is no leader
                ( leader_id != self_id && // or leader is not this node
                          ( host_group.FindHostById(leader_id)->GetStatus() != HostStatus::kLeader // and the leader status is not correct
                            || GetEpochSec() - last_live > 3
                                  // or leader is not live for 3 seconds
                          ) ) ){

            if (leader_id == -1)
                cout << "no leader" << endl;
            else if ( leader_id != self_id && host_group.FindHostById(leader_id)->GetStatus() != HostStatus::kLeader)
                cout << leader_id << " is not leader " << host_group.FindHostById(leader_id)->GetStatusStr() << endl;
            else if (leader_id != self_id && GetEpochSec() - last_live > 3)
                cout << "heart beat timeout" << endl;

            // start new election
            leader_id = elect();
            if (leader_id != -1 && leader_id != self_id) {
                host_group.FindHostById(leader_id)->SetLeader();
                last_live = GetEpochSec();
            }
        } else {
            if (leader_id != self_id) {
                // do heartbeat with remote leader
                Host *leader = host_group.FindHostById(leader_id);
                leader->SendMessage(to_string(self_id) + " " + to_string(view_number) + " HEARTBEAT 0");
            }
        }
        if (normal_msgs->wait_and_pop(msg, 1000))
            cout << msg.msg << " from " << msg.srcHost->GetHostName() << endl;

    }


    running = false;
    recv_thread.join();
}


