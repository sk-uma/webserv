#ifndef SOCKETCOLLECTION_HPP
# define SOCKETCOLLECTION_HPP

# include "ServerCollection.hpp"
# include "socket.hpp"

# include <vector>

class SocketCollection
{
  private:
    std::vector<Socket> socket_vector_;

  private:
    void InitSocket(const std::string &port, const std::string& address, const webservconfig::Server &server);

  public:
    SocketCollection();
    SocketCollection(const webservconfig::ServerCollection &config);
    ~SocketCollection();
    SocketCollection(const SocketCollection &other);
    SocketCollection &operator=(const SocketCollection &rhs);
};

#endif
