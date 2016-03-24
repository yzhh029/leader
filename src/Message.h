//
// Created by Yzhh on 2016/3/20.
//

#ifndef LEADER_MESSAGE_H
#define LEADER_MESSAGE_H

#include "Host.h"
#include <sstream>

// format: cli_id pp_num msg

class Host;

struct Message {

    const static int PP_BASE = 100000;
    Message() = default;

    Message(std::string recv_msg) {
        std::stringstream ss(recv_msg);
        int view_pp;
        ss >> cli_id >> view_pp >> msg >> value;
        pp_num = view_pp % PP_BASE;
        view_num = view_pp / PP_BASE;
    }
    Message(int dest_id, int _view_num, std::string send_msg) {
        msg = std::string(std::to_string(dest_id) + " " + std::to_string(_view_num) + " " + send_msg + " 0");
    }

    Message(int dest_id, int _view_num, std::string send_msg, std::string _value) {
        msg = std::string(std::to_string(dest_id) + " " + std::to_string(_view_num) + " " + send_msg + " " + _value);
    }

    Message(int dest_id, int _view_num, int _pp_num, std::string cmd, std::string _value) {
        std::string _view_pp(std::to_string(_view_num * PP_BASE + _pp_num));
        msg = std::string(std::to_string(dest_id) + " " + _view_pp + " " + cmd + " " + _value);
    }

    std::string msg;
    Host* srcHost;

    int pp_num;
    int view_num;
    int cli_id;
    std::string value;
};

#endif //LEADER_MESSAGE_H
