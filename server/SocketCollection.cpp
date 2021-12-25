# include "SocketCollection.hpp"

SocketCollection::SocketCollection():
  socket_vector_()
{ }

SocketCollection::SocketCollection(const webservconfig::ServerCollection &config)
{
  for (std::vector<webservconfig::Server>::const_iterator iter = config.GetServer().begin();
       iter != config.GetServer().end(); iter++) {
    std::string server_name = iter->GetServerName();
    for (webservconfig::ConfigBase::listen_type::const_iterator it = iter->GetListenV4().begin();
         it != iter->GetListenV4().end(); it++) {
      std::cout << server_name << " => " << it->first << ":" << it->second << std::endl;
    }
    for (webservconfig::ConfigBase::listen_type::const_iterator it = iter->GetListenV6().begin();
         it != iter->GetListenV6().end(); it++) {
      std::cout << server_name << " => " << it->first << ":" << it->second << std::endl;
    }
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
