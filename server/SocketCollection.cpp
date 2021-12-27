# include "SocketCollection.hpp"

SocketCollection::SocketCollection():
  socket_vector_()
{ }

SocketCollection::SocketCollection(const webservconfig::ServerCollection &config)
{
  // for (std::vector<webservconfig::Server>::const_iterator iter = config.GetServer().begin();
  //      iter != config.GetServer().end(); iter++) {
  //   std::string server_name = iter->GetServerName();
  //   for (webservconfig::ConfigBase::listen_type::const_iterator it = iter->GetListenV4().begin();
  //        it != iter->GetListenV4().end(); it++) {
  //     std::cout << server_name << " => " << it->first << ":" << it->second << std::endl;
  //   }
  //   for (webservconfig::ConfigBase::listen_type::const_iterator it = iter->GetListenV6().begin();
  //        it != iter->GetListenV6().end(); it++) {
  //     std::cout << server_name << " => " << it->first << ":" << it->second << std::endl;
  //   }
  // }
  for (std::vector<webservconfig::Server>::const_iterator iter = config.GetServer().begin();
       iter != config.GetServer().end(); iter++) {
    // std::string server_name = iter->GetServerName();
    iter->PutServerName(std::cout, "");
    // for (webservconfig::ConfigBase::listen_type::const_iterator it = iter->GetListen().begin();
    //      it != iter->GetListen().end(); it++) {
    //   std::cout << **it << " (" << std::endl;
    // }
    AddServer(*iter);
  }
}

SocketCollection::~SocketCollection()
{ }

SocketCollection::SocketCollection(const SocketCollection &other)
{
  *this = other;
}

SocketCollection &SocketCollection::operator=(const SocketCollection &rhs)
{
  if (this != &rhs) {
    this->socket_vector_ = rhs.socket_vector_;
  }
  return (*this);
}

const std::vector<Socket> &SocketCollection::GetSocket() const { return (this->socket_vector_); }

bool SocketCollection::CompareToAddr(const struct addrinfo *first, const struct addrinfo *second)
{
  if (first->ai_addr->sa_family == second->ai_addr->sa_family) {
    // int port1 = webservconfig::get_in_port(first->ai_addr);
    // int port2 = webservconfig::get_in_port(second->ai_addr);
    // char s1[INET6_ADDRSTRLEN];
    // char s2[INET6_ADDRSTRLEN];
    // const char *res1 = inet_ntop(first->ai_family, webservconfig::get_in_addr((struct sockaddr *)first->ai_addr), s1, sizeof(s1));
    // const char *res2 = inet_ntop(second->ai_family, webservconfig::get_in_addr((struct sockaddr *)second->ai_addr), s2, sizeof(s2));
    // if (port1 != port2) {
    //   return (true);
    // } else if (std::string(res1) != std::string(res2)) {
    //   return (true);
    // }
  } else {
    return (true);
  }
  return (false);
}

void SocketCollection::AddServer(const webservconfig::Server &server)
{
  webservconfig::ConfigBase::listen_string_type::const_iterator iter_s = server.GetListenString().begin();
  for (webservconfig::ConfigBase::listen_type::const_iterator iter = server.GetListen().begin();
       iter != server.GetListen().end() && iter_s != server.GetListenString().end(); iter++) {
    bool find_flag = false;
    std::cout << "in for 1" << std::endl;
    if (this->socket_vector_.size() != 0) {
      std::cout << this->socket_vector_.begin()->get_port() << std::endl;
      std::cout << "vec size: " << this->socket_vector_.size() << std::endl;
      for (std::vector<Socket>::iterator it = this->socket_vector_.begin();
          it != this->socket_vector_.end(); it++) {
        std::cout << "a";
        if (!CompareToAddr((it->get_ai()), *iter)) {
          it->add_server(server);
          find_flag = true;
        }
      }
    }
    std::cout << "in for 2" << std::endl;
    if (!find_flag) {
      std::cout << iter_s->first << ":" << iter_s->second << std::endl;
      this->socket_vector_.push_back(Socket(iter_s->first, iter_s->second));
      (this->socket_vector_.end() - 1)->add_server(server);
      (this->socket_vector_.end() - 1)->set_socket();
      fcntl((this->socket_vector_.end() - 1)->get_listenfd(), F_SETFL, O_NONBLOCK);
    }
    std::cout << "in for 3" << std::endl;
    iter_s++;
  }
}
