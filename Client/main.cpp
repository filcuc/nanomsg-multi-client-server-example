#include <iostream>
#include <cassert>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

using namespace std;


int main(int argc, char *argv[])
{
    int bytes = 0;
    int eid = -1;

    const std::string port = argc <= 1 ? "5761" : argv[1];

    std::string login_socket_endpoint {"tcp://127.0.0.1:5760"};
    std::string receive_socket_endpoint {"tcp://127.0.0.1:" + port};

    int login_socket = nn_socket(AF_SP, NN_REQ);
    assert(login_socket >= 0);
    eid = nn_connect(login_socket, login_socket_endpoint.c_str());
    assert(eid >= 0);

    std::cout << "[Client] Connect result is " << eid << std::endl;
    bytes = nn_send(login_socket,
                    receive_socket_endpoint.c_str(),
                    receive_socket_endpoint.length(), 0);
    std::cout << "[Client] Sent " << receive_socket_endpoint << " of " << bytes << "bytes" << std::endl;

    nn_shutdown(login_socket, eid);

    std::cout << "[Client] Creating the receive socket" << std::endl;
    int receive_socket = nn_socket(AF_SP, NN_REP);
    assert(receive_socket >= 0);
    eid = nn_bind(receive_socket, receive_socket_endpoint.c_str());
    assert(eid >= 0);

    std::cout << "[Client] Wait for server to connect" << std::endl;

    for (int i = 0; i < 10; ++i) {
        char* buf = nullptr;
        nn_recv(receive_socket, &buf, NN_MSG, 0);
        std::cout << "[Client] Received " << buf << " from the server" << std::endl;
    }
}
