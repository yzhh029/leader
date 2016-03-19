//
// Created by yzhh0 on 3/10/2016.
//

#include "Election.h"
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>


using namespace std;

Election::Election(std::vector<Host> hlist, string p)
        : host_group(std::move(hlist)), leader(nullptr), port(p) {

    char hostname[64];
    if (gethostname(hostname, 64) == -1) {
        perror("get hostname faild");
    }

    else {
     cout << hostname << endl;
    }
}

void Election::InitNet() {

    struct addrinfo hints, *servinfo, *p;

    bzero(&hints, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port.c_str(), &hints, &servinfo) != 0) {
        perror("server getaddrinfo failed");
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server sock failed");
            continue;
        }

        int enable =1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof enable) < 0) {
            perror("resue sock addr failed");
            continue;
        }

        if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("server bind error");
            continue;
        }
        break;
    }

    if ( p == NULL) {
        cout << "faild to bind socket" << endl;
        exit(1);
    }

    freeaddrinfo(servinfo);
    cout << "initialized for server " << port << endl;
}

Message Election::GetMessage() {

    struct sockaddr_storage client_addr;
    char buf[MAXBUFF];
    socklen_t addr_len;

    int size = recvfrom(sock, buf, MAXBUFF-1, 0, (struct sockaddr *)&client_addr, &addr_len);
    if (size == -1) {
        perror("receive from error");
        exit(1);
    }

    Message msg;
    msg.msg = string(buf, size);
    msg.addr = ((struct sockaddr_in *) &client_addr)->sin_addr.s_addr;

    cout << "recv " << msg.msg << endl;

    return msg;
}

Election::~Election() {
    close(sock);
}
