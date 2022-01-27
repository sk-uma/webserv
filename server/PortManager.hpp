#ifndef PORTMANAGER_HPP
# define PORTMANAGER_HPP

# include "Server.hpp"
# include "ServerCollection.hpp"
# include "Socket.hpp"

class PortManager
{
  public:
    typedef std::vector<Socket> socket_list_type;

  public:
    int                         port_;
    bool                        is_inaddr_any_;
    socket_list_type            socket_vector_;
    std::vector<struct in_addr> address_vector_;

  private:
    bool    IsInaddrAny(struct in_addr addr) const;
    Socket  *GetSocket(const webservconfig::ConfigBase::listen_type &address);

  public:
    PortManager();
    ~PortManager();
    PortManager(const PortManager &other);
    const PortManager &operator=(const PortManager &rhs);
    int AddSocket(const webservconfig::ConfigBase::listen_type &address,
                  const webservconfig::ConfigBase::listen_string_type &str_address,
                  const webservconfig::Server &server);

/**
 * Getter
 */

    const socket_list_type  &GetSocket() const;
    int                     GetPort() const;
};

std::ostream &operator<<(std::ostream &os, const PortManager &pm);

#endif
