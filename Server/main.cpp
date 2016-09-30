#include <iostream>
#include <cassert>
#include <thread>
#include <algorithm>
#include <vector>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

using namespace std;

void handle_client(const std::string& url) {
    std::cout << "[Server] Handling client at url " << url << std::endl;
    int socket = nn_socket(AF_SP, NN_REQ);
    assert(socket >= 0);
    int eid = nn_connect(socket, url.c_str());
    assert(eid >= 0);
    for (int i = 0; i < 10; ++i) {
        std::string message { to_string(i) + ") Hello " + url + " this is the server :)" };
        std::cout << "[Server] Sending message to client " << url << std::endl;
        nn_send(socket, message.c_str(), message.size(), 0);
        std::this_thread::sleep_for(1s);
    }
    nn_shutdown(socket, eid);
}

int main(int argc, char *argv[])
{
    std::vector<std::thread> threads;

    int s = nn_socket(AF_SP, NN_REP);
    assert(s >= 0);

    int eid = nn_bind(s, "tcp://127.0.0.1:5760");
    assert(eid >= 0);

    while(true) {
        char *buf = nullptr;
        std::cout << "[Server] Waiting a message" << std::endl;
        int bytes = nn_recv(s, &buf, NN_MSG, 0);
        std::cout << "[Server] Received " << bytes << " bytes" << std::endl;
        if (bytes < 0)
            continue;
        std::thread t([buf](){handle_client(buf);});
        threads.push_back(std::move(t));
        nn_freemsg(buf);
    }

    for (std::thread& t : threads)
        if (t.joinable())
            t.join();

    nn_shutdown(s, eid);
    return 0;
}
