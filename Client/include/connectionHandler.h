#ifndef CONNECTION_HANDLER__
#define CONNECTION_HANDLER__
                                           
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <list>
#include <unordered_map>
#include <boost/algorithm/string.hpp>

using namespace std;


using boost::asio::ip::tcp;

class ConnectionHandler {
private:
	const std::string host_;
    const short port_;
	boost::asio::io_service io_service_;   // Provides core I/O functionality
	tcp::socket socket_;
    unordered_map<string,string> books_prevOwner_map;
    bool connected;
    unordered_map<string,list<string>*> topic_books_map;
    unordered_map<int,string> receiptId_topic_map;
    unordered_map<int,string> receiptId_command_map;
    bool key_board_can_run;
    string get_word_after(string word_before, vector<string> msg_vector);
    bool should_wait;
public:
    bool isShouldWait() const;

public:
    bool getKeyBoardCanRun() const;

private:

    string userName;
    list<string> booksToBorrow;

public:
    ConnectionHandler(std::string host, short port);
    virtual ~ConnectionHandler();
 
    // Connect to the remote machine
    bool connect();

    // Read a fixed number of bytes from the server - blocking.
    // Returns false in case the connection is closed before bytesToRead bytes can be read.
    bool getBytes(char bytes[], unsigned int bytesToRead);
 
	// Send a fixed number of bytes from the client - blocking.
    // Returns false in case the connection is closed before all the data is sent.
    bool sendBytes(const char bytes[], int bytesToWrite);
	
    // Read an ascii line from the server
    // Returns false in case connection closed before a newline can be read.
    bool getLine(std::string& line);
	
	// Send an ascii line from the server
    // Returns false in case connection closed before all the data is sent.
    bool sendLine(std::string& line);
 
    // Get Ascii data from the server until the delimiter character
    // Returns false in case connection closed before null can be read.
    bool getFrameAscii(std::string& frame, char delimiter);
 
    // Send a message to the remote host.
    // Returns false in case connection is closed before all the data is sent.
    bool sendFrameAscii(const std::string& frame, char delimiter);
	
    // Close down the connection properly.
    void close();

    void run();

    void send(std::string);

    string getBookPrevOwner(string);
    void addBook(string topic,string book_name);
    void removeBook(string topic,string book_name);

    void add_to_topic_rcpt_map(string topic, int receipt_id);
    void add_to_rcptId_cmmnd_map(int id,string command);
    void remove_from_rcptId_cmmnd_map(int id);
    void setConnected(bool);
    bool is_connected();
    void setUserName(string name);
    void addNewListForNewTopic(string topic);

    void addBookToBorrow(string bookName);

    void removeBookToBorrow(string bookName);
}; //class ConnectionHandler
 
#endif