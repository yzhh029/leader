//
// Created by yzhh0 on 3/22/2016.
//

#include "utils.h"
#include <chrono>
#include <ctime>
#include <iostream>

using namespace std;

string Now() {
    auto now = time(nullptr);
    auto tm = *localtime(&now);

    char buf[15];
    int size = strftime(buf, 15, "[%T]", &tm);

    return string(buf, size);
}

void Print(string node, string msg) {

    cout << Now() << " Node " << node << ": " << msg << endl;
}

