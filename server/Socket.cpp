#include "Socket.hpp"

Socket::Socket(void):
  listenfd_(-1),
  server_()
{ }

// Socket::Socket(const std::string &address, const std::string &port):
//   listenfd_(-1),
//   str_port_(port),
//   port_(-1),
//   str_address_(address),
//   server_()
// {
//   struct addrinfo	*ai;

//   if (this->SetSockaddr_(&ai) == -1) {
//     throw std::runtime_error("getaddrinfo() failed.");
//   }
//   if (this->SetAddressAndPort_(ai) == -1) {
//     freeaddrinfo(ai);
//     throw std::runtime_error("inet_ntop() failed.");
//   }
//   freeaddrinfo(ai);
// }

Socket::Socket(const webservconfig::ConfigBase::listen_type &address,
               const webservconfig::ConfigBase::listen_string_type &str_address):
  listenfd_(-1),
  address_(address),
  str_address_(str_address),
  server_()
{
  struct addrinfo	*ai;

  if (this->SetSockaddr_(&ai) == -1) {
    throw std::runtime_error("getaddrinfo() failed.");
  }
  if (this->NormalizationAddress_(ai) == -1) {
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
    // this->str_port_ = rhs.str_port_;
    // this->port_ = rhs.port_;
    this->address_ = rhs.address_;
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
  res = getaddrinfo(this->str_address_.first.c_str(), this->str_address_.second.c_str(), &hints, ai);
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

int Socket::NormalizationAddress_(struct addrinfo *ai)
{
  char res[INET6_ADDRSTRLEN];

  if (!inet_ntop(ai->ai_family, webservconfig::get_in_addr((struct sockaddr *)ai->ai_addr), res, sizeof(res))) {
    return (-1);
  }
  this->str_address_ = std::make_pair(std::string(res), this->str_address_.second);
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
    // std::cerr << std::strerror(errno) << ": socket() failed." << std::endl;
    freeaddrinfo(ai);
    return (-1);
  }
  if (setsockopt(this->listenfd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) == -1) {
    // std::cerr << std::strerror(errno) << ": setsockopt() failed." << std::endl;
    close(this->listenfd_);
    freeaddrinfo(ai);
    return (-1);
  }
  if (bind(this->listenfd_,	ai->ai_addr, ai->ai_addrlen) == -1) {
    // std::cerr << std::strerror(errno) << ": bind() failed." << std::endl;
    close(this->listenfd_);
    freeaddrinfo(ai);
    return (-1);
  }
  if (listen(this->listenfd_, SOMAXCONN) == -1) {
    // std::cerr << std::strerror(errno) << ": listen() failed." << std::endl;
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
const Socket::listen_type &Socket::GetAddress() const { return (this->address_); }
const std::string &Socket::GetStrPort() const { return (this->str_address_.second); }
int Socket::GetPort() const { return (this->address_.second); }
const std::string &Socket::GetStrIPAddress() const { return (this->str_address_.first); }
const Socket::server_list_type &Socket::GetServerVector() const { return (this->server_); }

Socket::listen_type Socket::GetSocketAddress_(int sockfd) const
{
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  int rc = getsockname(sockfd, (struct sockaddr*)&sin, &len);
  if (rc != 0) {
    throw std::runtime_error("unknown address.");
  }
  std::string host(inet_ntoa(sin.sin_addr));
  int port = ntohs(sin.sin_port);
  // std::cout << host << ":" << port << std::endl;
  return std::make_pair(sin.sin_addr, port);
}

// std::string get_local_addr(int sockfd) {
//   struct sockaddr_in sin;
//   socklen_t len = sizeof(sin);
//   int rc = getsockname(sockfd, (struct sockaddr*)&sin, &len);
//   if (rc != 0) {
//     // error
//     return "";
//   }
//   std::string host(inet_ntoa(sin.sin_addr));
//   // int port = ntohs(sin.sin_port);
//   return host + ':';
// }

webservconfig::Server Socket::SearchServer(int fd, const std::string &host) const
{
  std::vector<webservconfig::Server> sv;
  webservconfig::Server serv;
  webservconfig::Server any_serv;
  int ip_find = false, any_find = false;
  listen_type receive = GetSocketAddress_(fd);
  // std::cout << "receive port: " << receive.second << std::endl;

  // std::cout << "is any: " << (this->address_.first.s_addr == INADDR_ANY) << std::endl;
  for (Socket::server_list_type::const_iterator iter = this->server_.begin();
        iter != this->server_.end(); iter++) {
    // std::cout << iter->GetServerName().at(0) << ": " << iter->HasListen(receive) << ", " << iter->HasServerName(host) << std::endl;
    bool has_listen = iter->HasListen(receive);
    bool has_server_name = iter->HasServerName(host);
    bool has_any_addr = iter->HasListen(this->address_);
    // std::cout << iter->GetServerName().at(0) << ", " << host << ": ";
    // std::cout << has_listen << ", " << has_server_name << std::endl;
    // std::cout << (this->address_.first.s_addr == INADDR_ANY) << (receive.first.s_addr == INADDR_ANY) << !any_find << std::endl;
    if (has_listen && has_server_name) {
      return (*iter);
    } else if (has_listen && !ip_find) {
      serv = *iter;
      ip_find = true;
    } else if ((this->address_.first.s_addr == INADDR_ANY) && has_any_addr && !any_find) {
      any_serv = *iter;
      any_find = true;
    }
  }
  if (any_find && !ip_find) {
    return (any_serv);
  }
  if (!ip_find) {
    throw std::runtime_error("not found server");
  }
  return (serv);
}
