//
// Created by zivsini@wincs.cs.bgu.ac.il on 07/01/2020.
//

#include "../include/KeyBoardThread.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include "../include/connectionHandler.h"

void login(vector<std::string> vector);

void join(vector<std::string> vector);

using namespace std;



void KeyBoardThread::runKeyBoard() {
//    while (true) {
    string first_input; // we know it's gonna be login command
    getline(cin, first_input);
    while(first_input.substr(0,5)!="login")
    {
        getline(cin,first_input);
    }
    std::vector<std::string> msg_input;
    boost::split(msg_input, first_input, boost::is_any_of(" "));
    string keyboard_command = msg_input.at(0);
    string input_host_port = msg_input.at(1);
    vector<string> host_port;
    boost::split(host_port, input_host_port, boost::is_any_of(":"));
    string host = host_port.at(0);
    string port = host_port.at(1);
    userName = msg_input.at(2);
    string password = msg_input.at(3);
    this->handler = new ConnectionHandler(host, stol(port));
    if (!handler->connect()) {
        cout << "Could not connect to server" << endl;
        //TODO: close everything and shut or something
    }
    thread handler_thread(&ConnectionHandler::run, handler);
    handler->setUserName(userName);
    string connect_stomp_message = "CONNECT\n"
                                   "accept-version:1.2\n"
                                   "host:stomp.cs.bgu.ac.il\n"
                                   "login:" + userName + "\n" +
                                   "passcode:" + password + "\n\n" +
                                   "\0";
    handler->sendFrameAscii(connect_stomp_message, '\0');
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        if (handler->getKeyBoardCanRun())
//            terminated=false;

    while(handler->isShouldWait()==true){}
    if(!handler->getKeyBoardCanRun())
    {
        terminated=true;
    }


    while (!terminated) {
        if (!handler->is_connected()) {
            terminated = true;
            break;
        } else {
            string input;
            getline(cin, input);
            std::vector<std::string> msg_input;
            boost::split(msg_input, input, boost::is_any_of(" "));
            string keyboard_command = msg_input.at(0);
            if (keyboard_command == "login") { login(msg_input); }
            else if (keyboard_command == "join") { join(msg_input); }
            else if (keyboard_command == "exit") { exit(msg_input); }
            else if (keyboard_command == "add") { add(msg_input); }
            else if (keyboard_command == "borrow") { borrow(msg_input); }
            else if (keyboard_command == "return") { fReturn(msg_input); }
            else if (keyboard_command == "status") { status(msg_input); }
            else if (keyboard_command == "logout") { logout(); }
            msg_input.clear();

        }
    }
    //  if (handler->getKeyBoardCanRun())
//    cout<<"handler thread joined"<<endl;
//            handler->setConnected(false);
//    handler_thread.interrupt();
    handler_thread.join();
    delete handler;
    delete(this);
}
//}


void KeyBoardThread::login(vector<string> msg_input) {
    string input_host_port = msg_input.at(1);
    vector<string> host_port;
    boost::split(host_port, input_host_port, boost::is_any_of(":"));
    string host = host_port.at(0);
    string port = host_port.at(1);
    userName = msg_input.at(2);
    string password = msg_input.at(3);
//    this->handler = new ConnectionHandler(host, stol(port));
//    if (!handler->connect()) {
//        cout << "Could not connect to server" << endl;
//    } else {
//        thread handler_thread1(&ConnectionHandler::run, handler);
//        handler->setUserName(userName);
    string connect_stomp_message ="CONNECT\n"
                                  "accept-version:1.2\n"
                                  "host:stomp.cs.bgu.ac.il\n"
                                  "login:" + userName + "\n" +
                                  "passcode:" + password + "\n\n" +
                                  "\0";
    handler->sendFrameAscii(connect_stomp_message,'\0');
//        handler_thread1.join();
//        cout<<"thread of login func finished join"<<endl;
//    }
    msg_input.clear();
}

void KeyBoardThread::join(vector<string> msg_input){
    string topic = msg_input.at(1);
    string subscribe_stomp_message = "SUBSCRIBE\n"
                                     "destination:"+ topic+"\n" +
                                     "id:"+to_string(subscription_id)+"\n" +
                                     "receipt:"+to_string(receipt_id)+"\n\n"+
                                     "\0";
    handler->sendFrameAscii(subscribe_stomp_message,'\0');
    handler->addNewListForNewTopic(topic);
    this->topic_id_map[topic]=subscription_id;
    this->topic__receiptId_map[topic]=receipt_id;
    handler->add_to_topic_rcpt_map(topic,receipt_id);
    handler->add_to_rcptId_cmmnd_map(receipt_id,"sub");
    subscription_id++;
    receipt_id++;
    msg_input.clear();
}


void KeyBoardThread::exit(vector<string> msg_input) {
    string topic = msg_input.at(1);
    unordered_map<string,int>::const_iterator iter = topic_id_map.find(topic);
    int id = iter->second;
    unordered_map<string,int>::const_iterator it= topic__receiptId_map.find(topic);
    int receiptId = it->second;
    string unsubscribe_stomp_message = "UNSUBSCRIBE\n"
                                       "id:" + to_string(id) + "\n" +
                                       "receipt:"+to_string(receiptId)+"\n\n"+
                                       "\0";
    handler->sendFrameAscii(unsubscribe_stomp_message,'\0');
    handler->remove_from_rcptId_cmmnd_map(receiptId);
    handler->add_to_rcptId_cmmnd_map(receiptId,"unsub");
    this->topic_id_map.erase(topic);
    this->topic__receiptId_map.erase(topic);
    msg_input.clear();
}

void KeyBoardThread::add(vector<string> msg_input) {
    string topic = msg_input.at(1);
    string book_name;
    for (int i=2; i < msg_input.size(); i++){
        book_name+= msg_input.at(i) + " ";
    }
    book_name=book_name.substr(0,book_name.size()-1);   // delete the last space after the book name

    string sendMsg = "SEND\n"
                     "destination:"+ topic+"\n\n"+
                     userName+" has added the book "+book_name+"\n"+
                     "\0";
    handler->sendFrameAscii(sendMsg,'\0');
    handler->addBook(topic,book_name);
    msg_input.clear();
}

void KeyBoardThread::borrow(vector<string> msg_input) {
    string topic = msg_input.at(1);
    string book_name;
    for (int i=2; i < msg_input.size(); i++){
        book_name+= msg_input.at(i) + " ";
    }
    book_name=book_name.substr(0,book_name.size()-1);   // delete the last space after the book name
    string sendMsg = "SEND\n"
                     "destination:"+ topic+"\n\n"+
                     userName+" wish to borrow "+book_name+"\n"+
                     "\0";
    handler->sendFrameAscii(sendMsg,'\0');
    handler->addBookToBorrow(book_name);
    msg_input.clear();
}

void KeyBoardThread::fReturn(vector<string> msg_input) {
    string book_name;
    string topic = msg_input.at(1);
    for (int i = 2; i < msg_input.size() ; ++i) {
        book_name+= msg_input.at(i) + " ";
    }
    book_name = book_name.substr(0,book_name.size()-1);
    string sendMsg = "SEND\n"
                     "destination:"+ topic+"\n\n"+
                     "Returning "+book_name+" to "+handler->getBookPrevOwner(book_name) +"\n"+   /** get the userName we took the book from  */
                     "\0";
    handler->removeBook(topic,book_name);
    handler->sendFrameAscii(sendMsg,'\0');
    msg_input.clear();
}

void KeyBoardThread::status(vector<string> msg_input) {
    string sendMsg = "SEND\n"
                     "destination:" + msg_input.at(1) + "\n\n" +
                     "book status" + "\n" +
                     "\0";
    handler->sendFrameAscii(sendMsg,'\0');
    msg_input.clear();
}

void KeyBoardThread::logout() {
    string disconnect_stomp_message = "DISCONNECT\n"
                                      "receipt:" + to_string(receipt_id) + "\n\n" +
                                      "\0";
    handler->add_to_rcptId_cmmnd_map(receipt_id,"discon");
    handler->sendFrameAscii(disconnect_stomp_message,'\0');
    this->receipt_id++;
    this->terminated= true;


}

KeyBoardThread::KeyBoardThread():topic_id_map(),topic__receiptId_map(),subscription_id(0),receipt_id(0),handler(),userName(""),terminated(true){
    terminated=false;
}

KeyBoardThread::~KeyBoardThread() {
    topic_id_map.clear();
    topic__receiptId_map.clear();
    handler= nullptr;
}

//void KeyBoardThread::setTerminated(bool is_terminated) {
//    terminated= (is_terminated);
//
//}
