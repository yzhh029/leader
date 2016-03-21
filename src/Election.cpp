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
#include <sys/time.h>
#include <arpa/inet.h>


using namespace std;

Election::Election(std::vector<Host> hlist, string p, int _self_id)
        : host_group(HostManager(hlist)), leader_id(-1), self_id(_self_id), port(p) {

    char hostname[64];
    if (gethostname(hostname, 64) == -1) {
        perror("get hostname faild");
    }

    else {
     cout << hostname << endl;
    }
    cout << host_group.GetGroupSize() << endl;

    recv_msgs = make_shared<MessageQueue>();
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
    cout << " " << msg.cli_id << " " << msg.view_num << " " << msg.msg << endl;
    int cli_addr = client_addr.sin_addr.s_addr;

    msg.srcHost = host_group.FindHostByAddr(cli_addr);
    if (msg.srcHost == nullptr) {
        msg.srcHost = host_group.FindHostById(msg.cli_id);
    }

    if (msg.srcHost)
    //cout << "recv" << msg.msg << endl;
        cout << "recv " << msg.msg << " from " << msg.srcHost->GetHostName() << endl;
    else {
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
            ele->recv_msgs->push(msg);
            //msg.srcHost->SendMessage(msg.msg + " ACK");
        }
    }
    cout << "recv end" << endl;
}



void Election::run() {

    running = true;

    thread recv_thread(RecvLoop, this);
    Message msg;

    for (;;) {
        if (recv_msgs->wait_and_pop(msg, 1000))
            cout << msg.msg << " from " << msg.srcHost->GetHostName() << endl;
        else
            cout << "." ;
    }


    running = false;
    recv_thread.join();
}


