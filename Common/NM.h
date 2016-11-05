#include <nanomsg/nn.h>

namespace nm
{

class Connection
{
public:
    enum class Type { Connect, Bind };

    Connection(int socket_id, std::string const& url, Type type)
        : socket_id(socket_id)
        , connection_id(type == Type::Connect ? nn_connect(socket_id, url.c_str()) : nn_bind(socket_id, url.c_str()))
    {
        if (socket_id < 0)
            throw std::runtime_error("Connection failed");
    }

    ~Connection() { nn_shutdown(socket_id, connection_id); }

private:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    const int socket_id;
    const int connection_id;
};

class Socket
{
public:
    Socket(int type)
        : socket_id(nn_socket(AF_SP, type))
    {
        if (socket_id < 0)
            throw std::runtime_error("Socket creation failed");
    }

    ~Socket() { nn_close(socket_id); }

    void bind(const std::string& url) {
        connection.reset(new Connection(socket_id, url, Connection::Type::Bind));
    }

    void connect(const std::string& url) {
        connection.reset(new Connection(socket_id, url, Connection::Type::Connect));
    }

    void disconnect() {
        connection.reset();
    }

    void send(const std::string& message) {
        if (!connection)
            throw std::runtime_error("Socket is not connected");
        int bytes = nn_send(socket_id, message.c_str(), message.size(), 0);
        if (bytes != message.size())
            throw std::runtime_error("Sent less than expected");
    }

    std::string receive() {
        if (!connection)
            throw std::runtime_error("Socket is not connected");
        char* buf = nullptr;
        int bytes = nn_recv(socket_id, &buf, NN_MSG, 0);
        if (bytes < 0)
            throw std::runtime_error("Socket receive error");
        std::string result(buf);
        nn_freemsg(buf);
        return result;
    }

private:
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    const int socket_id;
    std::unique_ptr<Connection> connection;
};

}
