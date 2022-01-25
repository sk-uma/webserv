#ifndef SOCKETCOLLECTION_HPP
# define SOCKETCOLLECTION_HPP

# include "ServerCollection.hpp"
# include "Socket.hpp"
# include "ConfigUtils.hpp"
# include "PortManager.hpp"

# include <vector>

class SocketCollection
{
  public:
    typedef std::vector<Socket>         socket_list_type;
    typedef std::map<int, PortManager>  port_manager_list_type;

  private:
    socket_list_type        socket_vector_;
    port_manager_list_type  pm_;

  private:
    void InitSocket(const std::string &port, const std::string& address, const webservconfig::Server &server);
    bool CompareToAddr(const struct addrinfo *first, const struct addrinfo *second);
    void SetPortManager_(const webservconfig::Server &server);

  public:
    SocketCollection();
    SocketCollection(const webservconfig::ServerCollection &config);
    ~SocketCollection();
    SocketCollection(const SocketCollection &other);
    SocketCollection &operator=(const SocketCollection &rhs);


/**
 * Getter
 */

    const std::vector<Socket> &GetSocket() const;
    const Socket &GetSocket(const std::string &address, int port) const;

};

#endif
