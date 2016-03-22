//
// Created by Yzhh on 2016/3/19.
//

#include "HostManager.h"
#include <iostream>

using namespace std;


Host *HostManager::FindHostByAddr(int addr) {
    if (hosts.empty())
        return nullptr;

    for (auto &h: hosts) {
        if (h.isHost(addr)) {
            return &h;
        }
    }
    return nullptr;
}


void HostManager::HostIsLive(int id) {

    Host * h = FindHostById(id);
    if (h != nullptr) {
        h->SetLeader();
    }
}

void HostManager::InitAllNet() {
    if (!hosts.empty()) {
        for (auto &h : hosts) {
            h.InitNet();
        }
    } else {
        cout << "no available hosts in hostmanager" << endl;
    }
}


int HostManager::Boardcast(std::string msg) {

    for (auto &h : hosts) {
        cout << "send " << msg << " to " << h.GetHostName() << endl;
        h.SendMessage(msg);
    }
    return 0;
}


int HostManager::Boardcast(Message msg) {

    Boardcast(msg.msg);
    return 0;
}


Host *HostManager::FindHostById(int id) {
    auto it = hosts_lookup.find(id);
    if (it == hosts_lookup.end()) {
        return nullptr;
    } else {
        return it->second;
    }
}







