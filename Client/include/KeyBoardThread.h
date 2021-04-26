//
// Created by zivsini@wincs.cs.bgu.ac.il on 07/01/2020.
//

#ifndef SPL3_TASK_H
#define SPL3_TASK_H
#include "../include/KeyBoardThread.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include "../include/connectionHandler.h"


using namespace std;


class KeyBoardThread {
private:
    ~KeyBoardThread();
    unordered_map<string,int> topic_id_map;
    unordered_map<string,int> topic__receiptId_map;
    int subscription_id; // might need to lock this, DK
    int receipt_id; // might need// to lock this, DK
    ConnectionHandler* handler;
    void login(vector<string>);
    string userName;
    void join(vector<string>);
    void exit(vector<string>);
    void logout();
    void add(vector<string>);
    void borrow(vector<string>);
    void fReturn(vector<string>);
    void status(vector<string>);
    bool terminated;
public:
//    static void setTerminated(bool);
    KeyBoardThread ();
    void runKeyBoard();
};


#endif //SPL3_TASK_H
