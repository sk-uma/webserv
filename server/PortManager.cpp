#include "PortManager.hpp"

PortManager::PortManager():
  port_(-1),
  is_inaddr_any_(false),
  socket_vector_(),
  address_vector_()
{ }

PortManager::~PortManager()
{ }

PortManager::PortManager(const PortManager &other)
{
  *this = other;
}

const PortManager &PortManager::operator=(const PortManager &rhs)
{
  if (this != &rhs) {
    this->port_ = rhs.port_;
    this->is_inaddr_any_ = rhs.is_inaddr_any_;
    this->socket_vector_ = rhs.socket_vector_;
    this->address_vector_ = rhs.address_vector_;
  }
  return (*this);
}

bool PortManager::IsInaddrAny(struct in_addr addr) const
{
  return (addr.s_addr == INADDR_ANY);
}

Socket *PortManager::GetSocket(const webservconfig::ConfigBase::listen_type &address)
{
  for (socket_list_type::iterator iter = this->socket_vector_.begin();
       iter != this->socket_vector_.end(); iter++) {
    if (iter->GetAddress() == address) {
      return &(*iter);
    }
  }
  return (NULL);
}

int PortManager::AddSocket(const webservconfig::ConfigBase::listen_type &address,
                           const webservconfig::ConfigBase::listen_string_type &str_address,
                           const webservconfig::Server &server)
{
  this->port_ = address.second;
  if (this->is_inaddr_any_) {
    this->socket_vector_.at(0).AddServer(server);
  } else {
    Socket *res = NULL;
    if (this->IsInaddrAny(address.first)) {
      Socket any_addr_socket(address, str_address);
      /*** TODO: replace insert ****/
      for (socket_list_type::const_iterator iter = this->socket_vector_.begin();
           iter != this->socket_vector_.end(); iter++) {
        for (Socket::server_list_type::const_iterator it = iter->GetServerVector().begin();
             it != iter->GetServerVector().end(); it++) {
          any_addr_socket.AddServer(*it);
        }
      }
      any_addr_socket.AddServer(server);
      this->socket_vector_.clear();
      this->socket_vector_.push_back(any_addr_socket);
      this->is_inaddr_any_ = true;
    } else if ((res = GetSocket(address)) != NULL) {
      res->AddServer(server);
    } else {
      Socket socket(address, str_address);
      socket.AddServer(server);
      this->socket_vector_.push_back(socket);
    }
  }
  return (0);
  (void)server;
  (void)address;
  (void)str_address;
}

/**
 * Getter
 */

const PortManager::socket_list_type &PortManager::GetSocket() const { return (this->socket_vector_); }
int PortManager::GetPort() const { return (this->port_); }

std::ostream &operator<<(std::ostream &os, const PortManager &pm)
{
  os << pm.GetPort() << std::endl;
  for (PortManager::socket_list_type::const_iterator iter = pm.GetSocket().begin();
       iter != pm.GetSocket().end(); iter++) {
    std::cout << "  " << iter->GetStrIPAddress() << std::endl;
    for (Socket::server_list_type::const_iterator it = iter->GetServerVector().begin();
         it != iter->GetServerVector().end(); it++) {
      if (it->GetServerName().size() != 0) {
        std::cout << "    " << *it->GetServerName().begin() << std::endl;
      } else {
        std::cout << "    not found name" << std::endl;
      }
    }
  }
  return (os);
}

bool operator==(struct in_addr lhs, struct in_addr rhs)
{
  return !(std::memcmp(&lhs, &rhs, sizeof(struct in_addr)));
}
