//
// Created by zivsini@wincs.cs.bgu.ac.il on 08/01/2020.
//

#include <boost/thread.hpp>
#include "../include/KeyBoardThread.h"

int main (int argc, char *argv[]) {
    /// tryout
    cout<<"Client started"<<endl;
//    string input;
//    getline(cin, input);
//    vector<string> v;
//    boost::split(v, input, boost::is_any_of(" "));
//    cout<<v.at(0)<<endl;
//    cout<<v.at(1)<<endl;
// ip:
//    login 127.0.0.1:7778 ziv aaa





    /// real program
    KeyBoardThread* th = new KeyBoardThread();
    thread real_thread(&KeyBoardThread::runKeyBoard,th);
    real_thread.join();


    return 0;
}