//
// Created by Yzhh on 2016/3/20.
//

#ifndef LEADER_MESSAGE_H
#define LEADER_MESSAGE_H

#include "Host.h"
#include <sstream>

// format: cli_id view_num msg

class Host;

struct Message {
    Message() = default;

    Message(std::string recv_msg) {
        std::stringstream ss(recv_msg);
        ss >> cli_id >> view_num >> msg >> value;
    }
    Message(int dest_id, int _view_num, std::string send_msg) {
        msg = std::string(std::to_string(dest_id) + " " + std::to_string(_view_num) + " " + send_msg + " 0");
    }

    Message(int dest_id, int _view_num, std::string send_msg, std::string _value) {
        msg = std::string(std::to_string(dest_id) + " " + std::to_string(_view_num) + " " + send_msg + " " + _value);
    }

    int GetProposalNum() {
        return cli_id * 100 + cli_id;
    }

    std::string msg;
    Host* srcHost;

    int view_num;
    int cli_id;
    std::string value;
};

#endif //LEADER_MESSAGE_H
