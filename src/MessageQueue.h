//
// Created by Yzhh on 2016/3/20.
//

#ifndef LEADER_MESSAGEQUEUE_H
#define LEADER_MESSAGEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Message.h"
#include "Election.h"

//struct Message;

class MessageQueue {
public:
    MessageQueue() = default;
    void push(Message msg);
    bool wait_and_pop(Message &msg, int timeout);

    bool empty() const;

private:
    mutable std::mutex mtx;
    std::queue<Message> msgs;
    std::condition_variable cv;
};


#endif //LEADER_MESSAGEQUEUE_H
