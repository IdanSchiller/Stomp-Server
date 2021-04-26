#include "../include/connectionHandler.h"
#include "../include/KeyBoardThread.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread.hpp>




using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using namespace std;


ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_),key_board_can_run(false),should_wait(true){}

ConnectionHandler::~ConnectionHandler() {
    books_prevOwner_map.clear();
    receiptId_command_map.clear();
    receiptId_topic_map.clear();
    booksToBorrow.clear();
    auto iter = topic_books_map.begin();
    int map_size = topic_books_map.size();
    while(iter!=topic_books_map.end())
    {
        iter->second->clear();
        delete (iter->second);
        iter->second=nullptr;
        topic_books_map.erase(iter->first);
        iter= topic_books_map.begin();
    }
    bool empty = topic_books_map.empty();
//    cout<<"Topic_books_map is empty? "+to_string(empty)<<endl;
    topic_books_map.clear();
    close();
}

bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
        else
            connected=true;
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
            tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}


bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        do{
            if(!getBytes(&ch, 1))
            {
                return false;
            }
            if(ch!='\0')
                frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}


bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    bool result=sendBytes(frame.c_str(),frame.length());
    if(!result) return false;
    return sendBytes(&delimiter,1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
        connected=false;
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }


}

void ConnectionHandler::run() {

    while (connected) {
        string answer_from_server;
//        getFrameAscii(answer_from_server,'\0');
        if (!getFrameAscii(answer_from_server,'\0')) {
            cout << "Disconnected. Exiting...\n" << endl;
            socket_.close();
            connected=false;
  //          break;
        } else {
            std::vector<std::string> answer_vector;
            boost::split(answer_vector, answer_from_server, boost::is_any_of("\n"));
            string stomp_command = answer_vector.at(0);
            if(stomp_command== "CONNECTED") {
                cout << "Login successful\n";
                this->key_board_can_run= true;
                should_wait= false;
            }
            else if(stomp_command=="MESSAGE") {
//                cout<<answer_from_server<< endl;
                string msg_body = answer_vector.at(5);
                string topic = get_word_after("destination:",answer_vector);
//                string topic = answer_vector.at(3);
//                int posOfColon = topic.find(":");
//                topic = topic.substr(posOfColon + 1);
                cout<<topic+":"+msg_body<<endl;
                if (msg_body.find("borrow") != -1) {
                   // cout<< userName+" got borrow command"<< endl;
                    unordered_map<string, list<string>* >::const_iterator iter = topic_books_map.find(topic);
                    size_t pos = msg_body.find("borrow");
                    string book_name = msg_body.substr(pos + 7, msg_body.size());
                    if (iter != topic_books_map.end()) {
                       // cout<< userName+" got borrow and found topic command - "+ book_name<< endl;
                        list<string> *tmpList = iter->second;
//                            list<basic_string<char>> *p = std::find(tmpList, tmpList + tmpList->size(), book_name);
                        for (string s : *tmpList) {
                            if (s == book_name) {
                               // cout<< userName+" got borrow and found book in library command"<< endl;
                                string sendMsg = +"SEND\n"
                                                  "destination:" + topic + "\n\n" +
                                                 userName + " has " + book_name + "\n" +
                                                 "\0";
                                sendFrameAscii(sendMsg,'\0');
//                                break;
                            }
                        }
                    }
                }
                else if (msg_body.find(" has ") != -1) {
                    size_t has_pos = msg_body.find("has");
                    string book_name = msg_body.substr(has_pos + 4, msg_body.size());
                    string userHasBook = msg_body.substr(0, has_pos - 1);
                    bool found_book=false;
                    for (string s:booksToBorrow) {
                        if (!found_book) {
                            if (s == book_name) {
                                found_book = true;
                                string sendMsg = "SEND\n"
                                                 "destination:" + topic + "\n\n" +
                                                 "Taking " + book_name + " from " + userHasBook + "\n" +
                                                 "\0";
                                sendFrameAscii(sendMsg, '\0');
                                addBook(topic, book_name);
//                            if(topic_books_map.find(topic)==topic_books_map.end()){
//                                list<string> *book_list = new list<string>;
//                                topic_books_map[topic]=book_list;
//                            }
//                            topic_books_map.at(topic)->push_back(book_name);
                                books_prevOwner_map[book_name] = userHasBook;
//                            break;
                            }
                        }
                    }
                    booksToBorrow.remove(book_name);

                }
                else if (msg_body.find("Taking") != -1) {
                    size_t from_pos = msg_body.find("from");
                    string user = msg_body.substr(from_pos + 5, msg_body.size());
                    int book_name_length = from_pos - 8;
                    string book_name = msg_body.substr(7, book_name_length);
                    if (user == this->userName) {
                        topic_books_map.at(topic)->remove(book_name);
                    }
//                    break;
                }
                else if (msg_body.find("Returning") != -1) {
                    size_t pos = msg_body.find_last_of(" to ");
                    int book_name_index = pos-13;
                    string book_name = msg_body.substr(10, book_name_index);
                    string user = msg_body.substr(pos + 1, msg_body.size());
                    if (user == this->userName) {
                        topic_books_map.at(topic)->push_back(book_name);
                    }
//                    break;
                }
                else if (msg_body.find("book status") != -1) {
                    //cout<< userName+" got status command"<< endl;
                    string booksList;
                    if (!topic_books_map.empty()) {
                        if(topic_books_map.find(topic)!=topic_books_map.end()) {
                            list<string>* booksInTheTopic = topic_books_map.at(topic);
                            for (string s: *booksInTheTopic) {
                                booksList += s + ",";
                            }
                            booksList = booksList.substr(0, booksList.size() - 1);
                            string sendMsg = "SEND\n"
                                             "destination:" + topic + "\n\n" +
                                             userName + ":" + booksList + "\n" +
                                             "\0";
                            sendFrameAscii(sendMsg, '\0');
//                            break;
                        }
                    }
                }

            }
            else if(stomp_command== "RECEIPT") {
//                string full_receipt_id = answer_vector.at(1);
//                int indexColon = full_receipt_id.find(":");
//                string rcpt_id = full_receipt_id.substr(indexColon+1);
                string rcpt_id = get_word_after("receipt-id:",answer_vector);
                int receipt_id = stoi(rcpt_id);
                unordered_map<int, string>::const_iterator it = receiptId_command_map.find(receipt_id);
                string command = it->second;
                if(command== "discon"){
                    socket_.close();
                    connected= false;
//                    cout<<"DISCONNECTED"<<endl;
                } /// something
                else if(command=="sub") {
                    unordered_map<int, string>::const_iterator it = receiptId_topic_map.find(receipt_id);
                    string topic = it->second;
                    cout << "Joined club " + topic << endl;
                }
                else if(command=="unsub"){
                    unordered_map<int, string>::const_iterator it = receiptId_topic_map.find(receipt_id);
                    string topic = it->second;
                    cout << "Exited club " + topic << endl;
                }
            }

            else if(stomp_command== "ERROR") {
                connected=false;
                should_wait=false;
                string error_msg = get_word_after("message:",answer_vector);
//                int indexColon = answer_vector.at(1).find(":");
//                string error_msg = answer_vector.at(1).substr(indexColon+1);
                cout << error_msg << endl;
                socket_.close();
                this->key_board_can_run=false;
               break;
//                if (error_msg=="Wrong password"){

 //               }
            }
        }
    }


}



void ConnectionHandler::send(string msg) {

}

string ConnectionHandler::getBookPrevOwner(string book_name) {
    unordered_map<string, string>::const_iterator it = books_prevOwner_map.find(book_name);
    string prev_owner = it->second;
    books_prevOwner_map.erase(book_name);
    return prev_owner;

}

void ConnectionHandler::addBook(string topic, string book_name) {
    unordered_map<string, list<string> *>::const_iterator it = topic_books_map.find(topic);
    if (it == topic_books_map.end()) {
        list<string> *book_list = new list<string>;
        book_list->push_back(book_name);
        topic_books_map[topic] = book_list;
    } else {
        it->second->push_back(book_name);

    }

}

void ConnectionHandler::add_to_topic_rcpt_map(string topic, int receipt_id) {
    receiptId_topic_map[receipt_id] = topic;

}

void ConnectionHandler::add_to_rcptId_cmmnd_map(int id, string command) {
    receiptId_command_map[id] = command;

}

void ConnectionHandler::remove_from_rcptId_cmmnd_map(int id) {
    this->receiptId_command_map.erase(id);

}

void ConnectionHandler::setUserName(string name) {
    this->userName = name;

}

void ConnectionHandler::addBookToBorrow(string bookName) {
    booksToBorrow.push_back(bookName);
}

void ConnectionHandler::removeBookToBorrow(string bookName) {
    booksToBorrow.remove(bookName);
}

bool ConnectionHandler::getKeyBoardCanRun() const {
    return key_board_can_run;
}

void ConnectionHandler::removeBook(string topic, string book_name) {
    auto it = topic_books_map.find(topic);
    if (it != topic_books_map.end()) {
        it->second->remove(book_name);
    }
}

void ConnectionHandler::addNewListForNewTopic(string topic) {
    unordered_map<string, list<string> *>::const_iterator it = topic_books_map.find(topic);
    if (it == topic_books_map.end()) {
        list<string> *book_list = new list<string>;
        topic_books_map[topic] = book_list;
    }
}

bool ConnectionHandler::is_connected() {
    return connected;
}

string ConnectionHandler::get_word_after(string word_before, vector<string> msg_vector) {
    int index_of_word_before=0;
    string word_after="found nothing";
    string contains_word_before="";
    for (string s:msg_vector){
        if (s.find(word_before) != std::string::npos) {
            index_of_word_before = s.find(word_before);
            contains_word_before=s;
        }
    }
    if(contains_word_before!="")
        word_after = contains_word_before.substr(index_of_word_before+word_before.length());


    return word_after;

}

bool ConnectionHandler::isShouldWait() const {
    return should_wait;
}


