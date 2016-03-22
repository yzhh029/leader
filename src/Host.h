//
// Created by yzhh0 on 3/9/2016.
//

#ifndef LEADER_HOST_H
#define LEADER_HOST_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "Message.h"
//#include <>

enum class HostStatus {
    kNormal,
    kLeader,
    kLost
};

class Message;
// a remote host
class Host {
public:
    explicit Host(std::string _hostname, int _id, std::string port);

    virtual ~Host();

    void SetLive() {status = HostStatus::kNormal; }
    void SetLeader() {status = HostStatus::kLeader; }
    HostStatus GetStatus() const { return status; }
    std::string GetStatusStr() const {
        if (status == HostStatus::kLeader) {
            return std::string("leader");
        } else if (status == HostStatus::kNormal) {
            return std::string("normal");
        } else if (status == HostStatus::kLost) {
            return std::string("lost");
        } else {
            return std::string("unknown");
        }
    }

    std::string GetHostName() const { return hostname; }
    int GetId() const { return id; }

    void SendMessage(std::string msg);
    void SendMessage(Message msg);

    bool isHost(const int address);
    void InitNet();
private:

    //void InitNet();

    int id;
    struct sockaddr_in addr;
    socklen_t  addrlen;
    HostStatus status;
    std::string port;
    int sock;
    std::string hostname;
};


#endif //LEADER_HOST_H
