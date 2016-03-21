//
// Created by yzhh0 on 3/9/2016.
//

#ifndef LEADER_HOST_H
#define LEADER_HOST_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
//#include <>

enum class HostStatus {
    kNormal,
    kLeader,
    kLost
};

// a remote host
class Host {
public:
    explicit Host(std::string _hostname, int _id, std::string port);

    virtual ~Host();


    std::string GetHostName() const { return hostname; }
    int GetId() const { return id; }

    void SendMessage(std::string msg);

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
