//
// Created by yzhh0 on 3/9/2016.
//

#include "Host.h"
#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>


using namespace std;

Host::Host(string _hostname, int _id, string _port) :
    hostname(_hostname), status(HostStatus::kLost), id(_id), sock(-1), port(_port)
{
    //if (hostname.find("\t", hostname.size() - 1) != string::npos)
    //    hostname.pop_back();
    bzero(&addr, sizeof addr);
    //InitNet();
}

// used for remote hosts
void Host::InitNet()  {

    struct addrinfo hints, *servinfo, *p;
    //struct addrinfo *res;

    bzero(&hints, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ( getaddrinfo(hostname.c_str(), port.c_str(), &hints, &servinfo) != 0) {
        perror((std::string("getaddrinof for") + hostname).c_str() );
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) {
            perror("sock init ");
            continue;
        }
        break;
    }

    if (p == NULL) {
        cout << "failed to bind socket for " << this->hostname << endl;
        exit(1);
    }

    addr = *(p->ai_addr);
    addrlen = p->ai_addrlen;
    cout << "initialized net for host " << hostname << endl;
    freeaddrinfo(servinfo);
}


void Host::SendMessage(std::string msg) {
    sendto(sock, msg.c_str(), msg.size(), 0, &addr, addrlen);
}

bool Host::isHost(const sockaddr_in &remote) {

    return remote.sin_addr.s_addr == ((struct sockaddr_in *)&addr)->sin_addr.s_addr;

}

Host::~Host() {
    close(sock);
}

