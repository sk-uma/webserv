#ifndef PORTMANAGER_HPP
# define PORTMANAGER_HPP

# include "Server.hpp"
# include "ServerCollection.hpp"
# include "Socket.hpp"

// template<typename addr_type>
// class PortManager
// {
//   public:
//     typedef std::vector<Socket> socket_list_type;

//   public:
//     int                     port_;
//     bool                    is_inaddr_any_;
//     u_char                  family_;
//     socket_list_type        socket_vector_;
//     std::vector<addr_type>  address_vector_;

//   private:
//     bool IsInaddrAny(addr_type addr) const
//     {
//       if (this->family_ == AF_INET) {
//         return INADDR_ANY == addr;
//       } else if (this->family_ == AF_INET6) {
//         return IN6ADDR_ANY_INIT == addr;
//       } else {
//         throw std::runtime_error("unknown address family.");
//       }
//       return (false);
//     }

//     Socket *GetSocket(const std::string &address, const std::string &port) const
//     {
//       for (socket_list_type::const_iterator iter = this->socket_vector_.begin();
//            iter != this->socket_vector_.end(); iter++) {
//         if (iter->GetStrPort() == port) {
//           ;
//         }
//       }
//       return ();
//     }

//   public:
//     PortManager():
//       port_(80),
//       is_inaddr_any_(false),
//       family_(AF_INET),
//       socket_vector_(),
//       address_vector_()
//     { }

//     PortManager(int port, u_char family):
//       port_(port),
//       is_inaddr_any_(false),
//       family_(family),
//       socket_vector_(),
//       address_vector_()
//     {
//       ;
//     }

//     ~PortManager()
//     { }

//     PortManager(const PortManager &other)
//     {
//       *this = other;
//     }

//     const PortManager &operator=(const PortManager &rhs)
//     {
//       if (this != &rhs) {
//         this->port_ = rhs.port_;
//         this->is_inaddr_any_ = rhs.is_inaddr_any_;
//         this->socket_vector_ = rhs.socket_vector_;
//         this->address_vector_ = rhs.address_vector_;
//       }
//       return (*this);
//     }

//     int AddSocket(addr_type addr, const webservconfig::Server &server,
//                   const std::string &address, const std::string &port)
//     {
//       if (this->is_inaddr_any_) {
//         this->socket_vector_.at(0).AddServer(server);
//       } else {
//         if (this->IsInaddrAny(addr)) {
//           Socket any_addr_socket(address, port);
//           this->socket_vector_.clear();
//           this->socket_vector_.push_back(any_addr_socket);
//         } else if () {
//           ;
//         }
//       }
//       return (0);
//       (void)server;
//     }
// };

#endif
