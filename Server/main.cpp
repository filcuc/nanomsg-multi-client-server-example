#include <iostream>
#include <cassert>
#include <thread>
#include <algorithm>
#include <vector>

#include <NM.h>
#include <nanomsg/reqrep.h>

using namespace std;

void handle_client(const std::string& url) {
    nm::Socket socket(NN_REQ);
    socket.connect(url);
    std::cout << "[Server] Handling client at url " << url << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::string message { to_string(i) + ") Hello " + url + " this is the server :)" };
        std::cout << "[Server] Sending message to client " << url << std::endl;
        socket.send(message);
        socket.receive();
        std::this_thread::sleep_for(1s);
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::thread> threads;

    nm::Socket socket(NN_REP);
    socket.bind("tcp://127.0.0.1:5760");

    while(true) {
        std::cout << "[Server] Waiting a message" << std::endl;
        std::string request = socket.receive();
        std::cout << "[Server] Received " << request << std::endl;
        socket.send("ack");
        std::thread t([request](){handle_client(request);});
        threads.push_back(std::move(t));
    }

    for (std::thread& t : threads)
        if (t.joinable())
            t.join();

    return 0;
}
