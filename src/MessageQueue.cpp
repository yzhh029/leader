//
// Created by Yzhh on 2016/3/20.
//

#include "MessageQueue.h"
#include <mutex>
#include <chrono>

using namespace std;

void MessageQueue::push(Message msg) {
    lock_guard<mutex> lock(mtx);
    msgs.push(msg);
    cv.notify_one();
}

bool MessageQueue::wait_and_pop(Message &msg, int timeout) {

    unique_lock<mutex> lock(mtx);

    if (cv.wait_for(lock, chrono::milliseconds(timeout), [this] {return !msgs.empty();}) ) {
        msg = std::move(msgs.front());
        msgs.pop();
        return true;
    } else {
        return false;
    }

}


bool MessageQueue::empty() const {
    lock_guard<mutex> lock(mtx);
    return msgs.empty();
}


