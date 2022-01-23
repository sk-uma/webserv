#include "PortManager.hpp"

// PortManager::PortManager():
//   port_(80),
//   is_inaddr_any_v4_(false),
//   is_inaddr_any_v6_(false),
//   socket_v4_(),
//   socket_v6_()
// { }

// PortManager::PortManager(int port):
//   port_(port),
//   is_inaddr_any_v4_(false),
//   is_inaddr_any_v6_(false),
//   socket_v4_(),
//   socket_v6_()
// {
//   ;
// }

// PortManager::~PortManager()
// { }

// PortManager::PortManager(const PortManager &other)
// {
//   *this = other;
// }

// const PortManager &PortManager::operator=(const PortManager &rhs)
// {
//   if (this != &rhs) {
//     this->port_ = rhs.port_;
//     this->is_inaddr_any_v4_ = rhs.is_inaddr_any_v4_;
//     this->is_inaddr_any_v6_ = rhs.is_inaddr_any_v6_;
//     this->socket_v4_ = rhs.socket_v4_;
//     this->socket_v6_ = rhs.socket_v6_;
//   }
//   return (*this);
// }

// int PortManager::AddSocket(struct in_addr addr, const webservconfig::Server &server)
// {
//   if (this->is_inaddr_any_v4_) {
//     this->socket_vector_.at(0).AddServer(server);
//   }
//   return (0);
//   (void)server;
// }

// int PortManager::AddSocket(struct in6_addr addr, const webservconfig::Server &server)
// {
//   if (this->is_inaddr_any_v6_) {
//     this->socket_vector_.at(0).AddServer(server);
//   }
//   return (0);
//   (void)server;
// }
