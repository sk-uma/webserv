#ifndef SOCKETCOLLECTION_HPP
# define SOCKETCOLLECTION_HPP

# include "ServerCollection.hpp"
# include "socket.hpp"
# include "ConfigUtils.hpp"

# include <vector>

class SocketCollection
{
  private:
    std::vector<Socket> socket_vector_;

  private:
    void InitSocket(const std::string &port, const std::string& address, const webservconfig::Server &server);

  protected:
    bool CompareToAddr(const struct addrinfo *first, const struct addrinfo *second);

  public:
    SocketCollection();
    SocketCollection(const webservconfig::ServerCollection &config);
    ~SocketCollection();
    SocketCollection(const SocketCollection &other);
    SocketCollection &operator=(const SocketCollection &rhs);

    void AddServer(const webservconfig::Server &server);

/**
 * Getter
 */

    const std::vector<Socket> &GetSocket() const;

};

#endif
