#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "Setting.hpp"
# include "ServerCollection.hpp"
# include "Server.hpp"

class Socket
{
  public:
    typedef std::vector<webservconfig::Server> server_list_type;

  private:
    int                                 listenfd_;
    std::string                         str_port_;
    int                                 port_;
    std::string                         str_address_;
    std::vector<webservconfig::Server>  server_;

  private:
    int SetSockaddr_(struct addrinfo **ai);
    int SetListenfd_(struct addrinfo *ai);
    int SetAddressAndPort_(struct addrinfo *ai);

  public:
    Socket(void);
    Socket(const std::string &address, const std::string &port);
    ~Socket();
    Socket(const Socket &other);
    const Socket &operator=(const Socket &rhs);

    int SetupSocket();
    void AddServer(const webservconfig::Server &s);

/**
 * Getter
 */

    int                     GetListenfd() const;
    const std::string       &GetStrPort() const;
    int                     GetPort() const;
    const std::string       &GetStrAddress() const;
    const server_list_type  &GetServerVector() const;
};

#endif
