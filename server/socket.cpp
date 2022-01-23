#include "Socket.hpp"

Socket::Socket(void):
  listenfd_(-1),
  str_port_(""),
  port_(-1),
  str_address_(""),
  server_()
{ }

Socket::Socket(const std::string &address, const std::string &port):
  listenfd_(-1),
  str_port_(port),
  port_(-1),
  str_address_(address),
  server_()
{
  struct addrinfo	*ai;

  if (this->SetSockaddr_(&ai) == -1) {
    throw std::runtime_error("getaddrinfo() failed.");
  }
  if (this->SetAddressAndPort_(ai) == -1) {
    freeaddrinfo(ai);
    throw std::runtime_error("inet_ntop() failed.");
  }
  freeaddrinfo(ai);
}

Socket::~Socket()
{ }

Socket::Socket(const Socket &other)
{
  *this = other;
}

const Socket &Socket::operator=(const Socket &rhs)
{
  if (this != &rhs) {
    this->listenfd_ = rhs.listenfd_;
    this->str_port_ = rhs.str_port_;
    this->port_ = rhs.port_;
    this->str_address_ = rhs.str_address_;
    this->server_ = rhs.server_;
  }
  return (*this);
}

int Socket::SetSockaddr_(struct addrinfo **ai)
{
  int res;
  struct addrinfo		hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  res = getaddrinfo(this->str_address_.c_str(), this->str_port_.c_str(), &hints, ai);
  return (res);
}

int Socket::SetListenfd_(struct addrinfo *ai)
{
  this->listenfd_ = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
  if (this->listenfd_ == -1) {
    return (-1);
  }
  return (0);
}

int Socket::SetAddressAndPort_(struct addrinfo *ai)
{
  char res[INET6_ADDRSTRLEN];

  this->port_ = webservconfig::get_in_port(ai->ai_addr);
  if (!inet_ntop(ai->ai_family, webservconfig::get_in_addr((struct sockaddr *)ai->ai_addr), res, sizeof(res))) {
    return (-1);
  }
  return (0);
}

int Socket::SetupSocket()
{
  int optval = 1, res;
  struct addrinfo *ai;

  if ((res = this->SetSockaddr_(&ai)) == -1) {
    std::cerr << res << ":" << gai_strerror(res) << std::endl;
    return (-1);
  }
  if (this->SetListenfd_(ai) == -1) {
    std::cerr << std::strerror(errno) << ": socket() failed." << std::endl;
    freeaddrinfo(ai);
    return (-1);
  }
  if (setsockopt(this->listenfd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) == -1) {
    std::cerr << std::strerror(errno) << ": setsockopt() failed." << std::endl;
    close(this->listenfd_);
    freeaddrinfo(ai);
    return (-1);
  }
  if (bind(this->listenfd_,	ai->ai_addr, ai->ai_addrlen) == -1) {
    std::cerr << std::strerror(errno) << ": bind() failed." << std::endl;
    close(this->listenfd_);
    freeaddrinfo(ai);
    return (-1);
  }
  if (listen(this->listenfd_, SOMAXCONN) == -1) {
    std::cerr << std::strerror(errno) << ": listen() failed." << std::endl;
    close(this->listenfd_);
    freeaddrinfo(ai);
    return (-1);
  }
  freeaddrinfo(ai);
  return (0);
}

void Socket::AddServer(const webservconfig::Server &s)
{
  this->server_.push_back(s);
}

/**
 * Getter
 */

int Socket::GetListenfd() const { return (this->listenfd_); }
const std::string &Socket::GetStrPort() const { return (this->str_port_); }
int Socket::GetPort() const { return (this->port_); }
const std::string &Socket::GetStrAddress() const { return (this->str_address_); }
const Socket::server_list_type &Socket::GetServerVector() const { return (this->server_); }
