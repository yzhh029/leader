//
// Created by Yzhh on 2016/3/20.
//

#ifndef LEADER_MESSAGE_H
#define LEADER_MESSAGE_H

#include "Host.h"
#include <sstream>

// format: cli_id view_num msg

struct Message {
    Message() = default;

    Message(std::string recv_msg) {
        std::stringstream ss(recv_msg);
        ss >> cli_id >> view_num >> msg;
    }
    Message(int dest_id, int _view_num, std::string send_msg) {
        msg = std::string(std::to_string(dest_id) + " " + std::to_string(_view_num) + " " + send_msg);

    }
    std::string msg;
    Host* srcHost;

    int view_num;
    int cli_id;
};

#endif //LEADER_MESSAGE_H
