//
// Created by Yzhh on 2016/3/19.
//

#ifndef LEADER_HOSTMANAGER_H
#define LEADER_HOSTMANAGER_H

#include <vector>
#include "Host.h"
#include <string>
#include <unordered_map>

class HostManager {
public:
    HostManager(std::vector<Host> _hosts) : hosts(std::move(_hosts)) {
        for (auto &h: hosts) {
            hosts_lookup[h.GetId()] = &h;
        }
    };
    void InitAllNet();
    int CheckLive();
    int GetLiveNum();
    int GetGroupSize() const { return hosts.size(); }
    int Boardcast(std::string msg);
    Host* FindHostByAddr(int addr);
    Host* FindHostById(int id);

private:
    std::vector<Host> hosts;
    std::unordered_map<int, Host*> hosts_lookup;
    int lastLiveNum;
};


#endif //LEADER_HOSTMANAGER_H
