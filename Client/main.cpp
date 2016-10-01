#include <iostream>
#include <cassert>
#include <memory>
#include <NM.h>
#include <nanomsg/reqrep.h>

using namespace std;

int main(int argc, char *argv[])
{
    int bytes = 0;
    int eid = -1;

    const std::string port = argc <= 1 ? "5761" : argv[1];
    std::string receive_socket_endpoint {"tcp://127.0.0.1:" + port};

    {
        std::string login_socket_endpoint {"tcp://127.0.0.1:5760"};
        nm::Socket login_socket(NN_REQ);
        login_socket.connect(login_socket_endpoint);
        std::cout << "[Client] Sending " << receive_socket_endpoint << std::endl;
        login_socket.send(receive_socket_endpoint);
        std::cout << "[Client] Sent " << receive_socket_endpoint << std::endl;
        std::string response = login_socket.receive();
        std::cout << "[Client] Received " << response << std::endl;
    }

    {
        std::cout << "[Client] Creating the receive socket" << std::endl;
        nm::Socket receive_socket(NN_REP);
        receive_socket.bind(receive_socket_endpoint);
        std::cout << "[Client] Wait for server to connect" << std::endl;
        for (int i = 0; i < 10; ++i) {
            std::string response = receive_socket.receive();
            std::cout << "[Client] Received " << response << " from the server" << std::endl;
            receive_socket.send("ack");
       }
    }
}
