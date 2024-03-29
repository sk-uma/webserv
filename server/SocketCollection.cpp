# include "SocketCollection.hpp"

SocketCollection::SocketCollection():
  socket_vector_(),
  pm_()
{ }

SocketCollection::SocketCollection(const webservconfig::ServerCollection &config):
  socket_vector_(),
  pm_()
{
  for (std::vector<webservconfig::Server>::const_iterator iter = config.GetServer().begin();
       iter != config.GetServer().end(); iter++) {
    if (CheckAddress_(*iter)) {
      throw std::runtime_error("setup the same port multiple times.");
    }
  }

  for (std::vector<webservconfig::Server>::const_iterator iter = config.GetServer().begin();
       iter != config.GetServer().end(); iter++) {
    SetPortManager_(*iter);
  }

  // for (port_manager_list_type::const_iterator iter = this->pm_.begin();
  //      iter != this->pm_.end(); iter++) {
  //   std::cout << iter->second;
  // }
  InitSocket_();
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

void SocketCollection::SetPortManager_(const webservconfig::Server &server)
{
  webservconfig::ConfigBase::listen_string_list_type::const_iterator iter_s = server.GetListenStringV4().begin();
  webservconfig::ConfigBase::listen_list_type::const_iterator iter = server.GetListenV4().begin();
  for (; iter != server.GetListenV4().end() && iter_s != server.GetListenStringV4().end(); iter++, iter_s++) {
    this->pm_[iter->second].AddSocket(*iter, *iter_s, server);
  }
}

void SocketCollection::InitSocket_()
{
  for (SocketCollection::port_manager_list_type::const_iterator iter = this->pm_.begin();
       iter != this->pm_.end(); iter++) {
    for (PortManager::socket_list_type::const_iterator it = iter->second.GetSocket().begin();
         it != iter->second.GetSocket().end(); it++) {
      Socket socket(*it);
      if (socket.SetupSocket() == -1) {
        throw std::runtime_error("failed to socket setup.");
      }
      this->socket_vector_.push_back(socket);
    }
  }
}

bool SocketCollection::CheckAddress_(const webservconfig::Server &server) const
{
  for (webservconfig::ConfigBase::listen_list_type::const_iterator iter = server.GetListenV4().begin();
       iter != server.GetListenV4().end(); iter++) {
    for (webservconfig::ConfigBase::listen_list_type::const_iterator it = iter+1; it != server.GetListenV4().end(); it++) {
      if (*iter == *it) {
        return (true);
      }
    }
  }
  return (false);
}

// const Socket &SocketCollection::GetSocket(const std::string &address, int port) const
// {
//   for (std::vector<Socket>::const_iterator iter = this->socket_vector_.begin();
//        iter != this->socket_vector_.end(); iter++) {
//     (void)iter;
//   }
//   (void)address;
//   (void)port;
//   return ();
// }

// void SocketCollection::AddServer(const webservconfig::Server &server)
// {
//   webservconfig::ConfigBase::listen_string_type::const_iterator iter_s = server.GetListenStringV4().begin();
//   for (webservconfig::ConfigBase::listen_v4_type::const_iterator iter = server.GetListenV4().begin();
//        iter != server.GetListenV4().end() && iter_s != server.GetListenStringV4().end(); iter++) {
//     bool find_flag = false;
//     if (this->socket_vector_.size() != 0) {
//       for (std::vector<Socket>::iterator it = this->socket_vector_.begin();
//           it != this->socket_vector_.end(); it++) {
//         if (it->get_port() == iter->second) {
//           // throw std::runtime_error("Duplicate ports.");
//         }
//       }
//     }
//     if (!find_flag) {
//       this->socket_vector_.push_back(Socket(iter_s->first, iter_s->second));
//       // (this->socket_vector_.end() - 1)->set_server(server);
//       (this->socket_vector_.end() - 1)->push_server(server);
//       int res = (this->socket_vector_.end() - 1)->set_socket();
//       if (res) {
//         throw std::runtime_error("socket error");
//       }
//       fcntl((this->socket_vector_.end() - 1)->get_listenfd(), F_SETFL, O_NONBLOCK);
//     }
//     iter_s++;
//   }
//   iter_s = server.GetListenStringV6().begin();
//   for (webservconfig::ConfigBase::listen_v6_type::const_iterator iter = server.GetListenV6().begin();
//        iter != server.GetListenV6().end() && iter_s != server.GetListenStringV6().end(); iter++) {
//     bool find_flag = false;
//     if (this->socket_vector_.size() != 0) {
//       for (std::vector<Socket>::iterator it = this->socket_vector_.begin();
//           it != this->socket_vector_.end(); it++) {
//         if (it->get_port() == iter->second) {
//           // throw std::runtime_error("Duplicate ports.");
//         }
//       }
//     }
//     if (!find_flag) {
//       this->socket_vector_.push_back(Socket(iter_s->first, iter_s->second));
//       // (this->socket_vector_.end() - 1)->set_server(server);
//       (this->socket_vector_.end() - 1)->push_server(server);
//       int res = (this->socket_vector_.end() - 1)->set_socket();
//       if (res) {
//         throw std::runtime_error("socket error");
//       }
//       fcntl((this->socket_vector_.end() - 1)->get_listenfd(), F_SETFL, O_NONBLOCK);
//     }
//     iter_s++;
//   }
//   (void)server;
// }

// void SocketCollection::AddServer(const webservconfig::Server &server)
// {
//   webservconfig::ConfigBase::listen_string_type::const_iterator iter_s = server.GetListenString().begin();
//   for (webservconfig::ConfigBase::listen_type::const_iterator iter = server.GetListen().begin();
//        iter != server.GetListen().end() && iter_s != server.GetListenString().end(); iter++) {
//     bool find_flag = false;
//     std::cout << "in for 1" << std::endl;
//     if (this->socket_vector_.size() != 0) {
//       std::cout << this->socket_vector_.begin()->get_port() << std::endl;
//       std::cout << "vec size: " << this->socket_vector_.size() << std::endl;
//       for (std::vector<Socket>::iterator it = this->socket_vector_.begin();
//           it != this->socket_vector_.end(); it++) {
//         std::cout << "a";
//         if (!CompareToAddr((it->get_ai()), *iter)) {
//           it->add_server(server);
//           find_flag = true;
//         }
//       }
//     }
//     std::cout << "in for 2" << std::endl;
//     if (!find_flag) {
//       std::cout << iter_s->first << ":" << iter_s->second << std::endl;
//       this->socket_vector_.push_back(Socket(iter_s->first, iter_s->second));
//       (this->socket_vector_.end() - 1)->add_server(server);
//       (this->socket_vector_.end() - 1)->set_socket();
//       fcntl((this->socket_vector_.end() - 1)->get_listenfd(), F_SETFL, O_NONBLOCK);
//     }
//     std::cout << "in for 3" << std::endl;
//     iter_s++;
//   }
//   (void)server;
// }
